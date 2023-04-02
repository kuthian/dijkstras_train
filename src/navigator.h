#ifndef NAVIGATOR_H
#define NAVIGATOR_H

#include <map>
#include <deque>

#include "board_square.h"

class navigator {
 public:
  navigator(const std::map<board_square*,
                           std::map<board_square*, direction_t> >& map);

  void initialize(const std::deque<board_square*>& path, board_square* start);

  board_square* current() const { return pos_; }
  direction_t dir() const { return next_.dir; }

  bool finished() const { return path_.empty(); }

  void step();

 private:
  next_square_op next_;
  board_square* pos_;
  board_square* dest_;
  std::deque<board_square*> path_;
  std::map<board_square*, std::map<board_square*, direction_t> > navigation_map_;
};

#endif