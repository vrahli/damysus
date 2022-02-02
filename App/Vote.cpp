#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Block.h"
#include "Accum.h"
#include "Void.h"
#include "Cert.h"
#include "Vote.h"


template <class B, class C>
void Vote<B,C>::serialize(salticidae::DataStream &data) const {
  data << this->cdata << this->sign;
}


template <class B, class C>
void Vote<B,C>::unserialize(salticidae::DataStream &data) {
  data >> this->cdata >> this->sign;
}


template <class B, class C>
Vote<B,C>::Vote() {}


template <class B, class C>
Vote<B,C>::Vote(salticidae::DataStream &data) {
  unserialize(data);
}


template <class B, class C>
Vote<B,C>::Vote(CData<B,C> cdata, Sign sign) {
  this->cdata=cdata;
  this->sign=sign;
}


template <class B, class C>
std::string Vote<B,C>::prettyPrint() {
  return ("VOTE[" + (this->cdata).prettyPrint()
          + "," + (this->sign).prettyPrint()
          + "]");
}

template <class B, class C>
std::string Vote<B,C>::toString() {
  return ((this->cdata).toString()
          + (this->sign).toString());
}


template <class B, class C>
CData<B,C> Vote<B,C>::getCData() { return this->cdata; }
template <class B, class C>
Sign       Vote<B,C>::getSign()  { return this->sign;  }


// instances of template:
template class Vote<Block,Accum>;
template class Vote<Hash,Void>;
template class Vote<Void,Cert>;
