#include "navigator.h"

navigator::navigator(
    const std::map<board_square*, std::map<board_square*, direction_t> >& map)
    : next_(direction_t::none),
      pos_(nullptr),
      dest_(nullptr),
      navigation_map_(map)
{
}

void navigator::initialize(const std::deque<board_square*>& train_path,
                           board_square* train_square)
{
  path_ = train_path;
  dest_ = path_.front();
  pos_ = train_square;
  next_ = next_square_op(pos_->piece()->train_dir());
}

void navigator::step()
{
  pos_ = next_(pos_);
  if (pos_ == dest_) {
    path_.pop_front();
    dest_ = path_.front();
    direction_t next_dir = navigation_map_[pos_][dest_];
    next_ = next_square_op(next_dir);
  }
};