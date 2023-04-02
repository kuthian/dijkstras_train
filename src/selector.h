#ifndef SELECTOR_H
#define SELECTOR_H

#include <QWidget>

#include "board_piece.h"

class selector : public QWidget {
  Q_OBJECT
 public:
  selector(QWidget *parent = nullptr);
 signals:
  void selection_changed(board_piece_t);
 private:
};

#endif