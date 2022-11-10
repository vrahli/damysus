#include "HJust.h"

HJust::HJust() {
  this->set  = false;
  this->hash = Hash();
  this->view = 0;
  this->auth = Auth();
}

HJust::HJust(Hash hash, View view, Auth auth) {
  this->set  = true;
  this->hash = hash;
  this->view = view;
  this->auth = auth;
}

HJust::HJust(bool b, Hash hash, View view, Auth auth) {
  this->set  = b;
  this->hash = hash;
  this->view = view;
  this->auth = auth;
}

bool HJust::isSet()   { return this->set;  }
Hash HJust::getHash() { return this->hash; }
View HJust::getView() { return this->view; }
Auth HJust::getAuth() { return this->auth; }


void HJust::serialize(salticidae::DataStream &data) const {
  data << this->set << this->hash << this->view << this->auth;
}


void HJust::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->hash >> this->view >> this->auth;
}


std::string HJust::prettyPrint() {
  return ("JUST[" + std::to_string(this->set)
          + "," + (this->hash).prettyPrint()
          + "," + std::to_string(this->view)
          + "," + (this->auth).prettyPrint() + "]");
}

std::string HJust::toString() {
  return (std::to_string(this->set)
          + (this->hash).toString()
          + std::to_string(this->view)
          + (this->auth).toString());
}
