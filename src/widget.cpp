#include "widget.h"

#include <QApplication>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QTextEdit>
#include <QTextStream>
#include <sstream>

#include "board.h"
#include "dijkstra.h"
#include "dimensions.h"
#include "selector.h"

widget::widget(QWidget* parent)
    : QWidget(parent), board_(new board), selector_(new selector)
{
  connect(board_, SIGNAL(finished()), this, SLOT(finished()));

  connect(selector_, SIGNAL(selection_changed(board_piece_t)), board_,
          SLOT(handle_selection_changed(board_piece_t)));

  QGridLayout* layout = new QGridLayout;
  layout->addWidget(board_, 0, 0);
  layout->addWidget(selector_, 1, 0, Qt::AlignHCenter);

  QHBoxLayout* controls_layout = new QHBoxLayout;
  {
    start_button_ = new QPushButton(tr("&Start"));
    connect(start_button_, SIGNAL(clicked()), this, SLOT(start()));
    controls_layout->addWidget(start_button_, Qt::AlignHCenter);

    clear_button_ = new QPushButton(tr("&Clear"));
    connect(clear_button_, SIGNAL(clicked()), board_, SLOT(clear()));
    controls_layout->addWidget(clear_button_, Qt::AlignHCenter);
  }
  layout->addLayout(controls_layout, 2, 0, Qt::AlignHCenter);

  QHBoxLayout* result_layout = new QHBoxLayout;
  result_ = new QTextEdit;
  result_->setFrameStyle(QFrame::Panel | QFrame::Sunken);
  result_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
  result_->setReadOnly(true);
  result_layout->addWidget(result_, 1);
  layout->addLayout(result_layout, 3, 0, Qt::AlignHCenter);

  setLayout(layout);
}

void widget::save()
{
  if (!savefile_.isEmpty()) {
    board_->save(savefile_);
  }
  else {
    save_as();
  }
}

void widget::save_as()
{
  QString savefile = QFileDialog::getSaveFileName(
      this, tr("Save File"), "board_state", tr("Save Files (*.sav)"));
  if (!savefile.isEmpty()) {
    if (!savefile.endsWith(".sav")) {
      savefile.append(".sav");
    }
    board_->save(savefile);
    savefile_ = savefile;
  }
}

void widget::open()
{
  QString savefile = QFileDialog::getOpenFileName(
      this, tr("Load Save File"), "board_state", tr("Save Files (*.sav)"));
  if (!savefile.isEmpty()) {
    board_->load(savefile);
    savefile_ = savefile;
  }
}

void widget::reload()
{
  if (!savefile_.isEmpty()) {
    board_->load(savefile_);
  }
}

std::string path_str(const std::deque<vertex>& path)
{
  std::ostringstream oss;
  bool first(true);
  for (const auto& v : path) {
    if (first)
      first = false;
    else
      oss << "-";
    oss << v;
  }
  return oss.str();
}

void widget::start()
{
  if (!board_->initialize()) return;

  const auto& graph = board_->graph();

  dijkstra_calculator dijkstra;
  dijkstra.calculate(*graph, {"S"});

  const std::unordered_map<vertex, int>& distances = dijkstra.distances();
  const std::unordered_map<vertex, std::deque<vertex>>& shortest_paths =
      dijkstra.shortest_paths();

  const std::vector<vertex> terminators = board_->terminators();

  std::unordered_map<vertex, int> term_distances;
  for (const vertex& t : terminators) {
    if (distances.count(t)) {
      term_distances[t] = distances.at(t);
    }
  }

  if (term_distances.empty()) {
    result_->setPlainText("Error: no valid paths to any terminators.");
    return;
  }

  auto closest_term = std::min_element(
      term_distances.begin(), term_distances.end(),
      [](const auto& l, const auto& r) { return l.second < r.second; });

  QString t;
  QTextStream txt(&t);
  txt << "[Vertex]\t[Distance]\t[Path]\n";
  for (const auto& d : term_distances) {
    txt << "S"
        << " -> " << d.first.c_str() << "\t" << d.second << "\t"
        << path_str(shortest_paths.at(d.first)).c_str();
    if (closest_term->first == d.first) {
      txt << "\t<------(Shortest Path)";
    }
    txt << "\n";
  }
  result_->setPlainText(*txt.string());

  QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  start_button_->setEnabled(false);
  clear_button_->setEnabled(false);
  selector_->setEnabled(false);
  board_->setEnabled(false);

  destination_ = QString(closest_term->first.c_str());
  board_->start(shortest_paths.at(closest_term->first));
}

void widget::finished()
{
  QApplication::restoreOverrideCursor();

  QMessageBox msgBox(this);
  msgBox.setWindowTitle("You have arrived!");
  msgBox.setText(QString("Congratulations, you have arrived at <b>Terminal %1</b>!")
                     .arg(destination_));
  msgBox.setStandardButtons(QMessageBox::Ok);
  msgBox.exec();

  start_button_->setEnabled(true);
  clear_button_->setEnabled(true);
  selector_->setEnabled(true);
  board_->setEnabled(true);
}
