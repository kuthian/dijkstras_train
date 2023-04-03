#include "board.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QLabel>
#include <QPainter>
#include <QThread>
#include <algorithm>
#include <map>
#include <queue>
#include <sstream>

#include "archiver.h"
#include "dimensions.h"
#include "navigator.h"

class node_registry {
 public:
  node_registry();
  board_piece* get_node(const std::string &key) const;
  void register_node(board_piece*);
  void deregister_node(board_piece*);
  bool full() const;
  void clear();

 private:
  enum { max_nodes = 26 };
  const std::string& next_available_key() const;
  std::map<std::string, board_piece*> nodes_;
};

node_registry::node_registry()
{
  for (int i = 0; i < max_nodes; i++) {
    std::ostringstream oss;
    oss << char('a' + i);
    nodes_[oss.str()] = nullptr;
  }
}

board_piece* node_registry::get_node(const std::string &key) const
{
  auto itr = nodes_.find(key);
  if (itr != std::end(nodes_)) {
    return itr->second;
  }
  return nullptr;
}

const std::string & node_registry::next_available_key() const
{
  auto available_key = [](const auto& n) { return n.second == nullptr; };
  auto itr = std::find_if(std::begin(nodes_), std::end(nodes_), available_key);
  return itr->first;
}

bool node_registry::full() const
{
  auto available_key = [](const auto& n) { return n.second == nullptr; };
  auto itr = std::find_if(std::begin(nodes_), std::end(nodes_), available_key);
  return itr == std::end(nodes_);
}

void node_registry::register_node(board_piece* piece)
{
  const std::string& key = next_available_key();
  piece->set_label(key);
  nodes_[key] = piece;
}

void node_registry::deregister_node(board_piece* piece)
{
  auto itr = nodes_.find(piece->label());
  if (itr != std::end(nodes_)) {
    itr->second = nullptr;
  }
}

void node_registry::clear()
{
  for (auto& n : nodes_) {
    n.second = nullptr;
  }
}

using namespace dimensions;

board::board(QWidget* parent)
    : QFrame(parent), node_registry_(new node_registry),
      train_(nullptr),
      selector_type_(board_piece_t::terminator)
{
  setMouseTracking(true);
  setFocusPolicy(Qt::StrongFocus);
  setFixedSize(columns * column_size, rows * row_size);
  setStyleSheet("background-color: rgb(150,150,150)");

  squares_.resize(columns);
  for (int col_i = 0; col_i < columns; col_i++) {
    for (int row_i = 0; row_i < rows; row_i++) {
      squares_[col_i].emplace_back(QPoint(col_i, row_i), squares_);
    }
  }
}

board::~board() = default;

void board::clear()
{
  board_pieces_.clear();
  for (int col_i = 0; col_i < columns; col_i++) {
    for (int row_i = 0; row_i < rows; row_i++) {
      squares_[col_i][row_i].clear();
    }
  }
  node_registry_->clear();
  train_ = nullptr;
  update();
}

void board::save(const QString& savefile)
{ 
  board_archiver::save(board_pieces_, savefile.toStdString());
}

void board::load(const QString& savefile)
{
  clear();
  board_archiver::load(board_pieces_, savefile.toStdString());
  for (const auto& bp : board_pieces_) {
    if (is_node(bp->type())) {
      node_registry_->register_node(bp.get());
    }
    square_at(bp->pos())->set(bp.get());
  }
  update();
}

void board::handle_selection_changed(board_piece_t piece_type)
{
  selector_type_ = piece_type;
}

void board::draw_grid()
{
  QPainter painter(this);
  painter.setPen(QPen(Qt::white, 2));
  for (int i = 0; i < width(); i += column_size) {
    painter.drawLine(i, 0, i, height());
  }

  for (int i = 0; i < height(); i += row_size) {
    painter.drawLine(0, i, width(), i);
  }
}

struct find_edge_op {
  edge operator()(board_square* sq, direction_t dir)
  {
    edge e;
    e.src = sq->piece()->label();
    e.weight = 0;
    next_square_op next(dir);
    while (next(sq) && next(sq)->occupied()) {
      e.weight++;
      sq = next(sq);
      if (is_node(sq->piece()->type())) break;
    }
    e.dest = sq->piece()->label();

    end = sq;
    return e;
  }

  board_square* end = nullptr;
};

bool board::initialize()
{
  if (!train_) return false;

  digraph_.reset();
  std::map<board_square*, std::map<board_square*, direction_t> > navigation_map;

  find_edge_op find_edge;
  std::vector<edge> edges;

  std::set<board_square*> visited;
  auto not_visited = [&visited] (board_square* b) { 
    return visited.count(b) == 0;
  };

  using nav_pair = std::pair<board_square*, direction_t>;
  std::queue<nav_pair> explore_queue;

  // Treat the train square as the starting position for
  // exploring the board outwardly.

  train_->set_label("S");

  board_square* start = square_at(train_->pos());
  direction_t start_dir = train_->train_dir();
  explore_queue.emplace(start, start_dir);

  while (!explore_queue.empty()) {
    nav_pair N = explore_queue.front();
    explore_queue.pop();
    board_square* sq = N.first; // board square that holds the node
    direction_t dir_taken = N.second; // direction taken to arrive at the node

    // Make sure we don't retrace our steps

    if (dir_taken != direction_t::down) {
      if (sq->up() && sq->up()->occupied()) {
        edges.push_back(find_edge(sq, direction_t::up));
        if (not_visited(find_edge.end)) {
          explore_queue.emplace(find_edge.end, direction_t::up);
        }
        navigation_map[sq][find_edge.end] = direction_t::up;
      }
    }
    if (dir_taken != direction_t::up) {
      if (sq->down() && sq->down()->occupied()) {
        edges.push_back(find_edge(sq, direction_t::down));
        if (not_visited(find_edge.end)) {
          explore_queue.emplace(find_edge.end, direction_t::down);
        }
        navigation_map[sq][find_edge.end] = direction_t::down;
      }
    }
    if (dir_taken != direction_t::right) {
      if (sq->left() && sq->left()->occupied()) {
        edges.push_back(find_edge(sq, direction_t::left));
        if (not_visited(find_edge.end)) {
          explore_queue.emplace(find_edge.end, direction_t::left);
        }
        navigation_map[sq][find_edge.end] = direction_t::left;
      }
    }
    if (dir_taken != direction_t::left) {
      if (sq->right() && sq->right()->occupied()) {
        edges.push_back(find_edge(sq, direction_t::right));
        if (not_visited(find_edge.end)) {
          explore_queue.emplace(find_edge.end, direction_t::right);
        }
        navigation_map[sq][find_edge.end] = direction_t::right;
      }
    }

    visited.insert(sq);
  }

  train_->set_label(""); // reset temporary label

  digraph_.emplace(edges);

  navigator_ = std::make_unique<navigator>(navigation_map);

  return true;
}

static int timer_id = 0;

void board::start( const std::deque<vertex>& vertex_path )
{
  std::deque<board_square*> path;
  for (const auto& v : vertex_path) {
    board_piece* node = node_registry_->get_node(v);
    if (node) {
      board_square* node_square = square_at(node->pos());
      path.push_back(node_square);
    }
  }

  navigator_->initialize(path, square_at(train_->pos()));

  timer_id = startTimer(1000);
}

bool board::try_add_piece(board_square& square)
{
  if (!square.occupied()) {
    board_square* up = square.up();
    board_square* down = square.down();
    board_square* left = square.left();
    board_square* right = square.right();

    std::unique_ptr<board_piece> piece_added;

    // Only junctions, terminators, and tracks may be placed on an an unoccupied square.

    switch(selector_type_)
    {
      case board_piece_t::junction:
      case board_piece_t::terminator:
        if (node_registry_->full()) {
          QMessageBox::warning(this, "Oops!", "Node limit reached, cannot add new node.");
          return false;
        }
        // Junctions and terminators (nodes) can be placed anywhere
        // and have no set orientation.
        piece_added = std::make_unique<board_piece>(selector_type_);
        piece_added->set_ori(orientation_t::none);
        node_registry_->register_node(piece_added.get());
        break;
      case board_piece_t::track:
        // Tracks can only be placed alongside a junction,
        // terminator, or another track.
        // New tracks will orient themselves so that their
        // orientation lines up with its neighbour.
        if ((up && up->occupied() &&
             up->piece()->ori() != orientation_t::horizontal) ||
            (down && down->occupied() &&
             down->piece()->ori() != orientation_t::horizontal)) {
          piece_added = std::make_unique<board_piece>(board_piece_t::track);
          piece_added->set_ori(orientation_t::vertical);
          break;
        }
        if ((left && left->occupied() &&
             left->piece()->ori() != orientation_t::vertical) ||
            (right && right->occupied() &&
             right->piece()->ori() != orientation_t::vertical)) {
          piece_added = std::make_unique<board_piece>(board_piece_t::track);
          piece_added->set_ori(orientation_t::horizontal);
          break;
        }
        break;
      default:
        break;
    }

    if (piece_added) {
      square.set(piece_added.get());
      board_pieces_.push_back(std::move(piece_added));
      return true;
    }
  }
  else {
    // Only trains can be placed on an occupied square.
    if (selector_type_ != board_piece_t::train) {
      return false;
    }
      
    board_piece* piece = square.piece();
    // Trains can only be placed on a track.
    if (piece->type() != board_piece_t::track) {
      return false;
    }

    // New trains will orient themselves so that their
    // direction lines up with the board piece it is placed on.
    // Clicking on an existing train will flip its direction.
    if (!piece->has_train()) {
      // Set new train
      if (piece->ori() == orientation_t::vertical) {
        piece->set_train_dir(direction_t::up);
      }
      if (piece->ori() == orientation_t::horizontal) {
        piece->set_train_dir(direction_t::left);
      }
      // Remove previous train
      if (train_ && train_ != piece) {
        train_->set_train_dir(direction_t::none);
      }
      train_ = piece;
    }
    else {
      piece->toggle_train_dir();
    }
    return true;
  }

  return false;
}

bool board::try_remove_piece(board_square& square)
{
  if (square.occupied()) {
    
    // If the train selector is being used, remove the train, not the board piece.
    if (selector_type_ == board_piece_t::train &&
        square.piece() == train_ ) {
      train_->clear_train();
      train_ = nullptr;
      return true;
    }

    auto same_piece = [&square](auto& i) { return i.get() == square.piece(); };
    auto itr = std::find_if(std::begin(board_pieces_), std::end(board_pieces_),
                            same_piece);
    if (itr != std::end(board_pieces_)) {
      if (is_node((*itr)->type())) {
        node_registry_->deregister_node(square.piece());
      }
      if (square.piece()->has_train()) {
        train_ = nullptr;
      }
      board_pieces_.erase(itr);
    }
    square.clear();
    return true;
  }

  return false;
}

std::vector<vertex> board::terminators() const
{
  std::vector<vertex> result;
  for (const auto& bp : board_pieces_) {
    if (bp->type() == board_piece_t::terminator) {
      result.push_back(bp->label());
    }
  }
  return result;
}

const std::optional<digraph>& board::graph() const { return digraph_; }

board_square* board::square_at( const QPoint& pos )
{
  board_square* result = nullptr;
  try {
    result = &squares_.at(pos.x()).at(pos.y());
  } catch( std::out_of_range&) {}
  return result;
}

void board::paintEvent(QPaintEvent* event)
{
  QFrame::paintEvent(event);

  draw_grid();

  QPainter painter(this);
  for (const auto& piece : board_pieces_) {
    piece->draw(painter);
  }

  if (mouse_pos_) {
    // Draw outline around the board square currently being hovered by the mouse
    QPoint top_left = dimensions::top_left(*mouse_pos_);
    painter.setPen(QPen(Qt::blue, 2));
    painter.drawRect(QRect(top_left, QSize(column_size, row_size)));
  }
}

void board::leaveEvent(QEvent*)
{
  if (mouse_pos_) {
    mouse_pos_.reset();
    update();
  }
}

void board::mouseMoveEvent(QMouseEvent* event)
{
  QPoint board_pos = dimensions::convert(event->pos());
  if (board_pos != mouse_pos_) {
    mouse_pos_ = board_pos;
    update();
  }
}

void board::mousePressEvent(QMouseEvent* event)
{
  bool any_change = false;
  if (event->button() == Qt::LeftButton) {
    QPoint board_pos = dimensions::convert(event->pos());
    board_square* square = square_at(board_pos);
    if (square) {
      any_change = try_add_piece(*square);
    }
  }
  if (event->button() == Qt::RightButton) {
    QPoint board_pos = dimensions::convert(event->pos());
    board_square* square = square_at(board_pos);
    if (square) {
      any_change = try_remove_piece(*square);
    }
  }

  if (any_change) {
    update();
  }
}

void board::timerEvent(QTimerEvent* event)
{
  if (navigator_->finished()) {
    train_ = nullptr;
    killTimer(timer_id);
    finished();
  } else {
    if (board_square* previous = navigator_->current()) {
      previous->piece()->set_train_dir(direction_t::none);
    }
    navigator_->step();
    if (board_square* current = navigator_->current()) {
      current->piece()->set_train_dir(navigator_->dir());
    }
  }
  update();
}
