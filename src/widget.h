#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class board;
class selector;
class QTextEdit;
class QPushButton;

class widget : public QWidget {
  Q_OBJECT
 public:
  widget(QWidget *parent = nullptr);
 signals:
  void save();
  void load();
 public slots:
  void start();
  void finished();

 private:
  board *board_;
  selector *selector_;
  QPushButton* start_button_;
  QPushButton* clear_button_;
  QTextEdit *result_;
};

#endif