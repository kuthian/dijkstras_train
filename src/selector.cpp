#include "selector.h"

#include <QHBoxLayout>
#include <QRadioButton>

selector::selector(QWidget *parent) : QWidget(parent)
{
  QRadioButton *terminator_rb = new QRadioButton(tr("Terminator"));
  connect(terminator_rb, &QRadioButton::clicked,
          [this](bool) { selection_changed(board_piece_t::terminator); });
  terminator_rb->setChecked(true);

  QRadioButton *track_rb = new QRadioButton(tr("Track"));
  connect(track_rb, &QRadioButton::clicked,
          [this]() { selection_changed(board_piece_t::track); });

  QRadioButton *junction_rb = new QRadioButton(tr("Junction"));
  connect(junction_rb, &QRadioButton::clicked,
          [this](bool) { selection_changed(board_piece_t::junction); });

  // // Not currently supported
  // QRadioButton *stoplight_rb = new QRadioButton(tr("Stoplight"));
  // connect(stoplight_rb, &QRadioButton::clicked,
  //         [this](bool) { selection_changed(board_piece_t::stoplight); });

  QRadioButton *train_rb = new QRadioButton(tr("Train"));
  connect(train_rb, &QRadioButton::clicked,
          [this](bool) { selection_changed(board_piece_t::train); });

  QHBoxLayout *layout = new QHBoxLayout;
  layout->addWidget(terminator_rb);
  layout->addWidget(track_rb);
  layout->addWidget(junction_rb);
  // layout->addWidget(stoplight_rb);
  layout->addWidget(train_rb);
  layout->addStretch(1);
  setLayout(layout);
}