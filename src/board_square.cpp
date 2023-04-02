#include "board_square.h"

board_square::board_square(const QPoint& pos,
                           std::vector<std::vector<board_square> >& squares)
    : pos_(pos), piece_(nullptr), squares_(squares)
{
}

void board_square::set(board_piece* piece)
{
  piece->set_pos(pos_);
  piece_ = piece;
};

board_square* board_square::up()
{
  if (y() == 0) return nullptr;
  return &squares_[x()][y() - 1];
}

board_square* board_square::down()
{
  if (y() >= squares_[0].size() - 1) return nullptr;
  return &squares_[x()][y() + 1];
}

board_square* board_square::left()
{
  if (x() == 0) return nullptr;
  return &squares_[x() - 1][y()];
}

board_square* board_square::right()
{
  if (x() >= squares_.size() - 1) return nullptr;
  return &squares_[x() + 1][y()];
}

board_square* next_square_op::operator()(board_square* sq)
{
  if (sq) {
    switch (dir) {
      case direction_t::up:
        return sq->up();
      case direction_t::down:
        return sq->down();
      case direction_t::left:
        return sq->left();
      case direction_t::right:
        return sq->right();
      default:
        return sq;
    }
  } else return nullptr;
}