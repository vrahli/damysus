#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "OPvote.h"


Hash    OPvote::getHash()  { return (this->hash);  }
View    OPvote::getView()  { return (this->view);  }
Auths   OPvote::getAuths() { return (this->auths); }


void OPvote::serialize(salticidae::DataStream &data) const {
  data << this->hash << this->view << this->auths;
}


void OPvote::unserialize(salticidae::DataStream &data) {
  data >> this->hash >> this->view >> this->auths;
}


OPvote::OPvote(Hash hash, View view, Auths auths) {
  this->hash=hash;
  this->view=view;
  this->auths=auths;
}


OPvote::OPvote() {}


void OPvote::insert(Auths auths) {
  this->auths.addNew(auths);
}


std::string OPvote::prettyPrint() {
  return ("PROP[" + (this->hash).prettyPrint()
          + "," + std::to_string(this->view)
          + "," + (this->auths).prettyPrint()
          + "]");
}

std::string OPvote::toString() {
  return ((this->hash).toString()
          + std::to_string(this->view)
          + (this->auths).toString());
}

std::string OPvote::data() {
  return ((this->hash).toString()
          + std::to_string(this->view));
}

bool OPvote::operator<(const OPvote& s) const {
  return view < s.view;
}
