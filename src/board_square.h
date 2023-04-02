#ifndef BOARD_SQUARE_H
#define BOARD_SQUARE_H

#include "board_piece.h"

class board_square {
 public:
  board_square(const QPoint& pos,
               std::vector<std::vector<board_square> >& squares);

  const QPoint& pos() const { return pos_; }
  int x() const { return pos_.x(); }
  int y() const { return pos_.y(); };

  void set(board_piece* piece);
  board_piece* piece() { return piece_; };
  board_piece* piece() const { return piece_; };

  bool occupied() const { return piece_; };
  void clear() { piece_ = nullptr; }

  // The following functions return board_squares in relation to our position on
  // the board. Will return a nullptr if navigating beyond the edge of the board.

  board_square* up();
  board_square* down();
  board_square* left();
  board_square* right();

 private:
  const QPoint pos_;
  board_piece* piece_;
  std::vector<std::vector<board_square> >& squares_;
};

// Utility functor for iterating over squares in a specified direction
struct next_square_op {
  next_square_op(direction_t d) { dir = d; }
  board_square* operator()(board_square* sq);
  direction_t dir;
};

#endif