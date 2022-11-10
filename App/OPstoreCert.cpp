#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "OPstoreCert.h"


View  OPstoreCert::getView()  { return (this->view);  }
Hash  OPstoreCert::getHash()  { return (this->hash);  }
View  OPstoreCert::getV()     { return (this->v);     }
Auths OPstoreCert::getAuths() { return (this->auths); }


void OPstoreCert::serialize(salticidae::DataStream &data) const {
  data << this->view << this->hash << this->v << this->auths;
}


void OPstoreCert::unserialize(salticidae::DataStream &data) {
  data >> this->view >> this->hash >> this->v >> this->auths;
}


OPstoreCert::OPstoreCert(View view, Hash hash, View v, Auths auths) {
  this->view=view;
  this->hash=hash;
  this->v=v;
  this->auths=auths;
}


OPstoreCert::OPstoreCert() {}


void OPstoreCert::insert(OPstore store) {
  if (this->auths.getSize() == 0) {
    this->view == store.getView();
    this->hash == store.getHash();
    this->v == store.getV();
    this->auths.add(store.getAuth());
  } else if (this->view == store.getView() && this->hash == store.getHash() && this->v == getV()) {
    this->auths.add(store.getAuth());
  }
}


std::string OPstoreCert::prettyPrint() {
  return ("STORE-CERT[" + std::to_string(this->view)
          + "," + (this->hash).prettyPrint()
          + "," + std::to_string(this->v)
          + "," + (this->auths).prettyPrint()
          + "]");
}

std::string OPstoreCert::toString() {
  return (std::to_string(this->view)
          + (this->hash).toString()
          + std::to_string(this->v)
          + (this->auths).toString());
}

std::string OPstoreCert::data() {
  return (std::to_string(this->view)
          + (this->hash).toString()
          + std::to_string(this->v));
}

bool OPstoreCert::operator<(const OPstoreCert& s) const {
  return view < s.view;
}
