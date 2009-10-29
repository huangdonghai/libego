//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef VERTEX_H_
#define VERTEX_H_

#include "config.h"

class Vertex : public Nat <Vertex> {
public:

  static const uint kBound = (board_size + 2) * (board_size + 2);

  // Constructors.

  explicit Vertex();

  static Vertex Pass();
  static Vertex Any(); // TODO remove it
  static Vertex Resign();

  static Vertex OfSgfString (const string& s);
  static Vertex OfGtpString (const string& s);
  static Vertex OfCoords (int row, int column); // TODO uint

  // Utilities.

  int GetRow() const;
  int GetColumn() const;

  // this can be achieved quicker by color_at lookup
  bool IsOnBoard() const;

  Vertex N() const;
  Vertex W() const;
  Vertex E() const;
  Vertex S() const;

  Vertex NW() const;
  Vertex NE() const;
  Vertex SW() const;
  Vertex SE() const;

  string ToGtpString() const;

private:
  friend class Nat <Vertex>;
  explicit Vertex (uint raw);
};

// TODO move this to GTP namespace
istream& operator>> (istream& in, Vertex& v);


#endif
