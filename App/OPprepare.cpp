#include "OPprepare.h"
#include <iostream>


OPprepare::OPprepare() {
  this->view  = 0;
  this->hash  = Hash();
  this->v     = 0;
  this->auths = Auths();
}

OPprepare::OPprepare(View view, Hash hash, View v, Auths auths) {
  this->view  = view;
  this->hash  = hash;
  this->v     = v;
  this->auths = auths;
}

View  OPprepare::getView()  { return this->view;  }
Hash  OPprepare::getHash()  { return this->hash;  }
View  OPprepare::getV()     { return this->v;     }
Auths OPprepare::getAuths() { return this->auths; }


void OPprepare::serialize(salticidae::DataStream &data) const {
  data << this->view << this->hash << this->v << this->auths;
}


void OPprepare::unserialize(salticidae::DataStream &data) {
  data >> this->view >> this->hash >> this->v >> this->auths;
}


void OPprepare::insert(OPstore store) {
  if (this->auths.getSize() == 0) {
    this->view = store.getView();
    this->hash = store.getHash();
    this->v = store.getV();
    this->auths.add(store.getAuth());
  } else if (this->view == store.getView() && this->hash == store.getHash() && this->v == getV()) {
    this->auths.add(store.getAuth());
  } else {
    if (DEBUG1) std::cout << KBLU << "OPprepare-insert-C:"
                          << (this->view == store.getView())
                          << ";" << (this->hash == store.getHash())
                          << ";" << this->hash.toString()
                          << ";" << store.getHash().toString()
                          << ";" << (this->v == getV())
                          << KNRM << std::endl;
  }
}

std::string OPprepare::prettyPrint() {
  return ("PREPARE[" + std::to_string(this->view)
          + "," + (this->hash).prettyPrint()
          + "," + std::to_string(this->v)
          + "," + (this->auths).prettyPrint() + "]");
}

std::string OPprepare::toString() {
  return (std::to_string(this->view)
          + (this->hash).toString()
          + std::to_string(this->v)
          + (this->auths).toString());
}

std::string OPprepare::data() {
  return (std::to_string(this->view)
          + (this->hash).toString()
          + std::to_string(this->v));
}

bool OPprepare::operator<(const OPprepare& s) const {
  return view < s.view;
}
