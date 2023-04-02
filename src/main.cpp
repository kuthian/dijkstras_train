#include <QApplication>
#include <QMainWindow>
#include <QMenuBar>

#include "widget.h"

int main(int argc, char *argv[])
{
  QApplication a(argc, argv);

  QMainWindow main_window;
  widget *main_widget = new widget;
  main_window.setCentralWidget(main_widget);

  auto fileMenu = main_window.menuBar()->addMenu("&File");

  QAction *save_action = new QAction("Save");
  save_action->setShortcuts(QKeySequence::Save);
  fileMenu->addAction(save_action);
  QObject::connect(save_action, &QAction::triggered, main_widget,
                   &widget::save);

  QAction *load_action = new QAction("Load");
  load_action->setShortcut(Qt::ControlModifier + Qt::Key_L);
  fileMenu->addAction(load_action);
  QObject::connect(load_action, &QAction::triggered, main_widget,
                   &widget::load);

  main_window.setWindowTitle("Dijkstra's Train");
  main_window.show();

  return a.exec();
}