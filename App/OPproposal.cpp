#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "OPproposal.h"


Hash OPproposal::getHash() { return (this->hash); }
View OPproposal::getView() { return (this->view); }
Auth OPproposal::getAuth() { return (this->auth); }


void OPproposal::serialize(salticidae::DataStream &data) const {
  data << this->hash << this->view << this->auth;
}


void OPproposal::unserialize(salticidae::DataStream &data) {
  data >> this->hash >> this->view >> this->auth;
}


OPproposal::OPproposal(Hash hash, View view, Auth auth) {
  this->hash=hash;
  this->view=view;
  this->auth=auth;
}


OPproposal::OPproposal() {
  this->hash=Hash(false);
  this->auth=Auth(false);
}


std::string OPproposal::prettyPrint() {
  return ("PROP[" + (this->hash).prettyPrint()
          + "," + std::to_string(this->view)
          + "," + (this->auth).prettyPrint()
          + "]");
}

std::string OPproposal::toString() {
  return ((this->hash).toString()
          + std::to_string(this->view)
          + (this->auth).toString());
}

std::string OPproposal::data() {
  return ((this->hash).toString()
          + std::to_string(this->view));
}

bool OPproposal::operator<(const OPproposal& s) const {
  return (auth < s.auth);
}
