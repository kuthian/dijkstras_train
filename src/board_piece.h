#ifndef BOARD_PIECE_H
#define BOARD_PIECE_H

#include <QPainter>
#include <QPoint>

enum class direction_t { none, up, down, left, right };
enum class orientation_t { none, vertical, horizontal };
enum class board_piece_t { terminator, track, junction, stoplight, train };

inline bool is_node(board_piece_t t) {
  return t == board_piece_t::terminator || 
         t == board_piece_t::junction;
}
 
class board_piece {
 public:
  board_piece();
  explicit board_piece(board_piece_t t);

  const std::string& label() const;
  void set_label(const std::string& l);

  board_piece_t type() const;
  void set_type(board_piece_t);

  const QPoint& pos() const;
  void set_pos(const QPoint&);

  orientation_t ori() const;
  void set_ori(orientation_t);

  bool has_train() const;
  direction_t train_dir() const;
  void set_train_dir(direction_t t);
  void toggle_train_dir();
  void clear_train();

  void draw(QPainter&) const;

 private:
  std::string label_;
  board_piece_t type_;
  orientation_t ori_;
  QPoint pos_;
  direction_t train_;
};

#endif