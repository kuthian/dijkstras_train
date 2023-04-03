#ifndef BOARD_H
#define BOARD_H

#include <QFrame>
#include <deque>
#include <memory>
#include <optional>
#include <vector>

#include "board_square.h"
#include "navigator.h"
#include "digraph.h"

class node_registry;
QT_FORWARD_DECLARE_CLASS(node_registry);

class board : public QFrame {
  Q_OBJECT
 public:
  board(QWidget* parent = nullptr);
  ~board();

  bool initialize();
  void start(const std::deque<vertex>& path);
  std::vector<vertex> terminators() const;
  const std::optional<digraph>& graph() const;

 signals:
  void finished();

 public slots:
  void save(const QString& savefile);
  void load(const QString& savefile);
  void clear();
  void handle_selection_changed(board_piece_t);

 protected:
  void leaveEvent(QEvent*) override;
  void mouseMoveEvent(QMouseEvent*) override;
  void mousePressEvent(QMouseEvent*) override;
  void paintEvent(QPaintEvent*) override;
  void timerEvent(QTimerEvent*) override;

 private:
  void draw_grid();
  bool try_add_piece(board_square&);
  bool try_remove_piece(board_square&);
  board_square* square_at(const QPoint&);

  std::optional<digraph> digraph_;

  std::unique_ptr<navigator> navigator_;

  std::unique_ptr<node_registry> node_registry_;

  std::vector<std::unique_ptr<board_piece> > board_pieces_;
  std::vector<std::vector<board_square> > squares_;
  board_piece* train_;

  board_piece_t selector_type_;
  std::optional<QPoint> mouse_pos_;
};

#endif