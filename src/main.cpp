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

  QAction *save_as_action = new QAction("Save As");
  save_as_action->setShortcut(Qt::ControlModifier + Qt::AltModifier + Qt::Key_S);
  fileMenu->addAction(save_as_action);
  QObject::connect(save_as_action, &QAction::triggered, main_widget,
                   &widget::save_as);

  QAction *open_action = new QAction("Open");
  open_action->setShortcut(Qt::ControlModifier + Qt::Key_O);
  fileMenu->addAction(open_action);
  QObject::connect(open_action, &QAction::triggered, main_widget,
                   &widget::open);

  QAction *load_action = new QAction("Reload");
  load_action->setShortcut(Qt::ControlModifier + Qt::Key_L);
  fileMenu->addAction(load_action);
  QObject::connect(load_action, &QAction::triggered, main_widget,
                   &widget::reload);

  main_window.setWindowTitle("Dijkstra's Train");
  main_window.show();

  return a.exec();
}