#include "board_piece.h"

#include "dimensions.h"

static constexpr int width = dimensions::column_size;
static constexpr int height = dimensions::row_size;

board_piece::board_piece()
    : ori_(orientation_t::none), train_(direction_t::none)
{
}

board_piece::board_piece(board_piece_t t)
    : type_(t), ori_(orientation_t::horizontal), train_(direction_t::none)
{
}

void board_piece::draw(QPainter &painter) const
{
  static constexpr QSize outer_size = QSize(width, height);
  static constexpr QSize size = QSize(width - 3, height - 3);

  static constexpr QRgb colors[4] = {
      0x333333,  // black
      0x808000,  // olive
      0x800080,  // teal
      0xffffff,  // white
  };

  const QColor color = colors[(int)type()];

  const QPoint top_left = dimensions::top_left(pos_);
  const QRect rect(top_left + QPoint(2, 2), size);

  painter.fillRect(rect, color);

  painter.setPen(QPen(color.lighter(), 2));
  painter.drawLine(rect.topLeft(), rect.bottomLeft());
  painter.drawLine(rect.topLeft(), rect.topRight());

  painter.setPen(QPen(color.darker(), 2));
  painter.drawLine(rect.topRight(), rect.bottomRight());
  painter.drawLine(rect.bottomLeft(), rect.bottomRight());

  if (type() == board_piece_t::track) {

    painter.setPen(QPen(Qt::white, 2));

    const QPoint center = rect.center() + QPoint(rect.width() % 2 ? 0 : 1,
                                                 rect.height() % 2 ? 0 : 1);
    switch (ori_) {
      case orientation_t::vertical:
        painter.drawLine(center + QPoint(0, 5), center - QPoint(0, 5));
        break;
      case orientation_t::horizontal:
        painter.drawLine(center + QPoint(5, 0), center - QPoint(5, 0));
        break;
      default:
        break;
    }
  }

  // Only display labels for nodes
  if (is_node(type())) {
    painter.setPen(QPen(Qt::white, 2));
    painter.drawText(rect, Qt::AlignHCenter | Qt::AlignVCenter, label_.c_str());
  }

  if (has_train()) {
    painter.setPen(QPen(Qt::black, 2));
    const QRect outer_rect = QRect(top_left, outer_size);
    painter.drawRect(outer_rect);

    const QPoint voffset = QPoint(0,3);
    const QPoint hoffset = QPoint(3,0);
    painter.setPen(QPen(Qt::black, 2));
    switch (train_) {
      case direction_t::up:
        painter.drawLine(outer_rect.topLeft() + voffset, 
                         outer_rect.topRight() + voffset);
        break;
      case direction_t::down:
        painter.drawLine(outer_rect.bottomLeft() - voffset,
                         outer_rect.bottomRight() - voffset);
        break;
      case direction_t::left:
        painter.drawLine(outer_rect.bottomLeft() + hoffset,
                         outer_rect.topLeft() + hoffset);
        break;
      case direction_t::right:
        painter.drawLine(outer_rect.bottomRight() - hoffset,
                         outer_rect.topRight() - hoffset);
        break;
      default:
        break;
    }
  }
}

void board_piece::toggle_train_dir()
{
  QString train_label;
  switch (train_) {
    case direction_t::up:
      train_ = direction_t::down;
      break;
    case direction_t::down:
      train_ = direction_t::up;
      break;
    case direction_t::left:
      train_ = direction_t::right;
      break;
    case direction_t::right:
      train_ = direction_t::left;
      break;
    default:
      break;
  }
}

bool board_piece::has_train() const { return train_ != direction_t::none; }
direction_t board_piece::train_dir() const { return train_; }
void board_piece::set_train_dir(direction_t t) { train_ = t; }
void board_piece::clear_train() { train_ = direction_t::none; }

const std::string &board_piece::label() const { return label_; }
void board_piece::set_label(const std::string &l) { label_ = l; }

board_piece_t board_piece::type() const { return type_; }
void board_piece::set_type(board_piece_t t) { type_ = t; }

const QPoint &board_piece::pos() const { return pos_; }
void board_piece::set_pos(const QPoint &p) { pos_ = p; }

orientation_t board_piece::ori() const { return ori_; }
void board_piece::set_ori(orientation_t o) { ori_ = o; }