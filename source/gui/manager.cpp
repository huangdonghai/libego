#include <QCheckBox>
#include <QDebug>
#include <QIntValidator>
#include <QPushButton>
#include <QtGui>

#include <iostream>

#include "GameScene.h"
#include "ResizableView.h"
#include "SquareGrid.h"

#include "engine.hpp"

#include "manager.h"

namespace Gui {

// -----------------------------------------------------------------------------

void vertex2gui (Vertex v, int& x, int& y) {
  x = v.GetColumn ()+1;
  y = board_size-v.GetRow ();
}

Vertex gui2vertex (int x, int y) {
  return Vertex::OfCoords (board_size-y, x-1);
}

// -----------------------------------------------------------------------------


Manager::Manager (Engine& engine) :
  QDialog (0),
  engine (engine)
{
  // Controlls on the right
  QVBoxLayout* controls = new QVBoxLayout;
  QWidget* right = new QWidget (this);
  right->setLayout (controls);

  controls->addWidget(new QLabel("<b>Libego</b>"), 0, Qt::AlignHCenter);

  QPushButton* clear_board = new QPushButton("Clear Board");
  controls->addWidget(clear_board);
  connect (clear_board, SIGNAL (clicked ()), this, SLOT (clearBoard ()));

  QPushButton* play_move = new QPushButton("Play move");
  controls->addWidget(play_move);
  connect (play_move, SIGNAL (clicked ()), this, SLOT (playMove ()));

  QPushButton* undo_move = new QPushButton ("Undo move");
  controls->addWidget (undo_move);
  connect (undo_move, SIGNAL (clicked ()), this, SLOT (undoMove ()));

  QPushButton* playout_move = new QPushButton ("Playout Move");
  controls->addWidget (playout_move);
  connect (playout_move, SIGNAL (clicked ()), this, SLOT (playoutMove ()));

  radio_nul    = new QRadioButton("Nothing", right);
  radio_mcts_n = new QRadioButton("MctsN", right);
  radio_mcts_m = new QRadioButton("MctsMean", right);
  radio_rave_n = new QRadioButton("RaveN", right);
  radio_rave_m = new QRadioButton("RaveMean", right);
  radio_bias   = new QRadioButton("Bias", right);
  radio_mix    = new QRadioButton("MctsPolicyMix", right);
  radio_samp_p = new QRadioButton("SamplerProb", right);
  radio_gamma  = new QRadioButton("PatternGammas", right);
  radio_gamma2 = new QRadioButton("CompleteGammas", right);
  radio_terr   = new QRadioButton("Territory", right);
  radio_terr2  = new QRadioButton("Territory with tree", right);
  controls->addWidget (radio_nul);
  controls->addWidget (radio_mcts_n);
  controls->addWidget (radio_mcts_m);
  controls->addWidget (radio_rave_n);
  controls->addWidget (radio_rave_m);
  controls->addWidget (radio_bias);
  controls->addWidget (radio_mix);
  controls->addWidget (radio_samp_p);
  controls->addWidget (radio_gamma);
  controls->addWidget (radio_gamma2);
  controls->addWidget (radio_terr);
  controls->addWidget (radio_terr2);
  connect (radio_nul,    SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_mcts_n, SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_mcts_m, SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_rave_n, SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_rave_m, SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_bias,   SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_mix,    SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_samp_p, SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_gamma,  SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_gamma2, SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_terr,   SIGNAL (clicked ()), this, SLOT (refreshBoard ()));
  connect (radio_terr2,  SIGNAL (clicked ()), this, SLOT (refreshBoard ()));

  radio_nul->setChecked (true);

  slider1 = new QSlider (this);
  slider2 = new QSlider (this);
  slider1->setOrientation (Qt::Horizontal);
  slider2->setOrientation (Qt::Horizontal);
  slider1->setMinimum (-100);
  slider2->setMinimum (-100);
  slider1->setMaximum (100);
  slider2->setMaximum (100);
  slider1->setValue (0);
  slider2->setValue (0);
  controls->addWidget (slider1);
  controls->addWidget (slider2);
  connect (slider1, SIGNAL (sliderMoved (int)), this, SLOT (sliderMoved (int)));
  connect (slider2, SIGNAL (sliderMoved (int)), this, SLOT (sliderMoved (int)));

  
  statebar = new QLabel (" ");
  statebar->setFixedWidth(200);
  QFont font("Monospace");
  font.setStyleHint(QFont::TypeWriter);
  statebar->setFont (font);
  controls->addWidget (statebar);

  controls->addStretch ();

  // Board.
  game_scene = GameScene::createGoScene(9);
  connect (game_scene, SIGNAL (mousePressed (int, int, Qt::MouseButtons)),
           this, SLOT (handleMousePress (int, int, Qt::MouseButtons)));
  connect (game_scene, SIGNAL (hooverEntered (int, int)),
           this, SLOT (handleHooverEntered (int, int)));


  // Main layout

  QHBoxLayout* mainLayout = new QHBoxLayout;
  QSplitter* main_widget = new QSplitter (this);
  mainLayout->addWidget(main_widget);
  main_widget->addWidget (new ResizableView(game_scene, this));
  main_widget->addWidget (right);
  

  setLayout (mainLayout);
  refreshBoard ();
}


void Manager::handleMousePress (int x, int y, Qt::MouseButtons buttons)
{
  if (! (buttons & Qt::LeftButton)) return;
  Vertex v = gui2vertex (x, y);
  engine.Play (Move (engine.GetBoard ().ActPlayer (), v));
  refreshBoard ();
}


void Manager::handleHooverEntered (int x, int y) {
  Vertex v = gui2vertex (x, y);
  QString hoover_text = engine.GetStringForVertex (v).c_str ();
  hoover_text += "%1\n";
  statebar->setText (hoover_text.arg (influence[v]));
}

void Manager::sliderMoved (int) {
  refreshBoard ();
}

void Manager::showGammas (int state)
{
  refreshBoard ();
}


void Manager::playMove ()
{
  engine.Genmove (engine.GetBoard ().ActPlayer ());
  refreshBoard ();
}

void Manager::clearBoard ()
{
  engine.Reset (board_size);
  refreshBoard ();
}


void Manager::undoMove ()
{
  engine.Undo ();
  refreshBoard ();
}

void Manager::playoutMove ()
{
  engine.DoPlayoutMove ();
  refreshBoard ();
}

void Manager::getInfluence () {
  influence_type = Engine::NoInfluence;
  if (radio_nul->isChecked())    influence_type = Engine::NoInfluence;
  if (radio_mcts_n->isChecked()) influence_type = Engine::MctsN;
  if (radio_mcts_m->isChecked()) influence_type = Engine::MctsMean;
  if (radio_rave_n->isChecked()) influence_type = Engine::RaveN;
  if (radio_rave_m->isChecked()) influence_type = Engine::RaveMean;
  if (radio_bias->isChecked())   influence_type = Engine::Bias;
  if (radio_mix->isChecked())    influence_type = Engine::MctsPolicyMix;
  if (radio_samp_p->isChecked()) influence_type = Engine::SamplerMoveProb;
  if (radio_gamma->isChecked())  influence_type = Engine::PatternGammas;
  if (radio_gamma2->isChecked()) influence_type = Engine::CompleteGammas;
  if (radio_terr->isChecked())   influence_type = Engine::PlayoutTerritory;
  if (radio_terr2->isChecked())  influence_type = Engine::MctsTerritory;
  engine.GetInfluence (influence_type, influence);
  ForEachNat (Vertex, v) {
    if (!v.IsOnBoard()) influence [v] = nan("");
  }
}

void Manager::refreshBoard ()
{
  std::cout << "refreshBoard ()" << std::endl;

  for (uint x=1; x<=board_size; x++) {
    for (uint y=1; y<=board_size; y++) {
      Vertex v = gui2vertex (x,y);
      Color col = engine.GetBoard ().ColorAt (v);

      if (col == Color::Empty ()) {
        game_scene->removeStone (x,y);
      } else if (col == Color::White ()) {
        game_scene->addWhiteStone (x,y);
      } else if (col == Color::Black ()) {
        game_scene->addBlackStone (x,y);
      }

      if (engine.GetBoard ().LastVertex () == v) {
        game_scene->addCircle (x,y);
      } else {
        game_scene->removeCircle (x,y);
      }
    }
  }

  //show gammas
  getInfluence ();
  NatMap <Vertex, double> hsv = influence;

  if (influence_type == Engine::MctsN ||
      influence_type == Engine::RaveN)
  {      
    hsv.Scale (-1, 1);
  }

  if (influence_type == Engine::SamplerMoveProb) {
    hsv /= hsv.Max () / 2;
    hsv -= 1;
  }

  if (influence_type == Engine::CompleteGammas ||
      influence_type == Engine::PatternGammas)
  {
    cerr << "MaxGamma = " << hsv.Max () << endl;
    hsv /= hsv.Max () / 2;
    hsv -= 1;
    //hsv.Dump ();
  }

  for (uint x=1; x<=board_size; x++) {
    for (uint y=1; y<=board_size; y++) {
      Vertex v = gui2vertex (x,y);
      double mean  = slider1->value() / 100.0;
      double scale = pow (100, slider2->value() / 100.0);
      double val = (hsv [v] - mean) * scale;
      val = max (val, -1.0);
      val = min (val, 1.0);
      if (!isnan (val)) {
        QColor color;
        color.setHsvF ((val + 1) / 6.0, 1.0, 1.0, 0.6);
        game_scene->addBGMark (x, y, color);
      } else {
        game_scene->removeBGMark (x, y);
      }
    }
  }
}

} // namespace
