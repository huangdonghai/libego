//
// Copyright 2006 and onwards, Lukasz Lew
//

#ifndef NAT_INL_H_
#define NAT_INL_H_

#include "test.hpp"

template <class T>
T Nat<T>::OfRaw (uint raw) {
  ASSERT (raw < T::kBound);
  return T (raw);
}

template <class T>
T Nat<T>::Invalid() {
  return T (-1);
}

template <class T>
Nat<T>::Nat () : raw (-1) {
}

// Utils.

template <class T>
bool Nat<T>::MoveNext() {
  if (raw+1 == T::kBound) {
    return false;
  }
  raw += 1;
  return true;
}

template <class T>
uint Nat<T>::GetRaw() const {
  return raw;
}

template <class T>
bool Nat<T>::IsValid() const {
  return *this != Invalid();
}

template <class T>
bool Nat<T>::operator == (const Nat& other) const {
  return this->raw == other.raw;
}

template <class T>
bool Nat<T>::operator != (const Nat& other) const {
  return this->raw != other.raw;
}

template <class T>
Nat<T>::Nat (uint raw) : raw (raw) {
  ASSERT (raw < T::kBound || raw == static_cast <uint> (-1));
}

// -----------------------------------------------------------------------------

template <typename Nat, typename Elt>
NatMap <Nat, Elt>::NatMap (const Elt& init) {
  SetAll (init);
}

template <typename Nat, typename Elt>
Elt& NatMap <Nat, Elt>::operator [] (Nat nat) { 
  ASSERT (nat != Nat::Invalid());
  return tab [nat.GetRaw()];
}

template <typename Nat, typename Elt>
const Elt& NatMap <Nat, Elt>::operator [] (Nat nat) const {
  ASSERT (nat != Nat::Invalid());
  return tab [nat.GetRaw()];
}

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::SetAll (const Elt& elt) {
  ForEachNat (Nat, nat) {
    (*this)[nat] = elt;
  }
}

template <typename Nat, typename Elt>
void NatMap <Nat, Elt>::SetToZero () { 
  memset (tab, '\0', sizeof (tab)); 
}

#endif