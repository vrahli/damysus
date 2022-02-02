#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "CData.h"
#include "Block.h"
#include "Hash.h"
#include "Accum.h"
#include "Cert.h"
#include "Void.h"


template <class B, class C>
void CData<B,C>::serialize(salticidae::DataStream &data) const {
  data << this->phase << this->view << this->block << this->cert;
}


template <class B, class C>
void CData<B,C>::unserialize(salticidae::DataStream &data) {
  data >> this->phase >> this->view >> this->block >> this->cert;
}


template <class B, class C>
CData<B,C>::CData() {}


template <class B, class C>
CData<B,C>::CData(salticidae::DataStream &data) {
  unserialize(data);
}


template <class B, class C>
CData<B,C>::CData(Phase1 phase, View view, B block, C cert) {
  this->phase=phase;
  this->view=view;
  this->block=block;
  this->cert=cert;
}


std::string ph2string(Phase1 phase) {
  switch (phase) {
  case PH1_NEWVIEW:
    return "PH_NEWVIEW";

  case PH1_PREPARE:
    return "PH_PREPARE";

  case PH1_PRECOMMIT:
    return "PH_PRECOMMIT";

  case PH1_COMMIT:
    return "PH_COMMIT";
  }

  return "";
}

template <class B, class C>
std::string CData<B,C>::prettyPrint() {
  return ("CDATA[" + ph2string(this->phase)
          + "," + std::to_string(this->view)
          + "," + (this->block).prettyPrint()
          + "," + (this->cert).prettyPrint()
          + "]");
}

template <class B, class C>
std::string CData<B,C>::toString() {
  return (std::to_string(this->phase)
          + std::to_string(this->view)
          + (this->block).toString()
          + (this->cert).toString());
}


template <class B, class C>
Phase1 CData<B,C>::getPhase() { return this->phase; }
template <class B, class C>
View   CData<B,C>::getView()  { return this->view;  }
template <class B, class C>
B      CData<B,C>::getBlock() { return this->block; }
template <class B, class C>
C      CData<B,C>::getCert()  { return this->cert;  }


template <class B, class C>
bool CData<B,C>::operator==(const CData<B,C>& s) const {
  return (phase == s.phase && view == s.view && block == s.block && cert == s.cert);
}


// instances of template:
template class CData<Block,Accum>;
template class CData<Hash,Void>;
template class CData<Void,Cert>;
