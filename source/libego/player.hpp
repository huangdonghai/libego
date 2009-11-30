//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef PLAYER_H_
#define PLAYER_H_

#include <istream>
#include <string>
#include "nat.hpp"

class Player : public Nat <Player> {
public:

  // Constructors.

  static Player Black ();
  static Player White ();
  static Player OfGtpStream (std::istream& s);

  // Utilities.

  Player Other() const;
  int ToScore () const;   // ToScore (Black()) == 1, ToScore (White()) == -1
  std::string ToGtpString () const;

  template <typename T>
  T SubjectiveScore (const T& score) const;

  // Other.

  static const uint kBound = 2;

 private:
  friend class Nat <Player>;
  explicit Player (uint raw);
};

#include "player-inl.hpp"

#endif