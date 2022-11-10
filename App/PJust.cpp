#include "PJust.h"

PJust::PJust() {
  this->hash  = Hash();
  this->view  = 0;
  this->auth  = Auth();
  this->auths = Auths();
}

PJust::PJust(bool b) {
  this->hash  = Hash(b);
  this->view  = 0;
  this->auth  = Auth(b);
  this->auths = Auths();
}

PJust::PJust(Hash hash, View view, Auth auth, Auths auths) {
  this->hash  = hash;
  this->view  = view;
  this->auth  = auth;
  this->auths = auths;
}

Hash  PJust::getHash()  { return this->hash;  }
View  PJust::getView()  { return this->view;  }
Auth  PJust::getAuth()  { return this->auth;  }
Auths PJust::getAuths() { return this->auths; }


void PJust::add(Auth a) {
  this->auths.add(a);
}

unsigned int PJust::sizeAuth() {
  int i = this->auths.getSize();
  if (this->hash.getSet() && this->auth.getHash().getSet()) { i++; }
    return i;
}


void PJust::serialize(salticidae::DataStream &data) const {
  data << this->hash << this->view << this->auth << this->auths;
}


void PJust::unserialize(salticidae::DataStream &data) {
  data >> this->hash >> this->view >> this->auth >> this->auths;
}


std::string PJust::prettyPrint() {
  return ("JUST[" + (this->hash).prettyPrint()
          + "," + std::to_string(this->view)
          + "," + (this->auth).prettyPrint()
          + "," + (this->auths).prettyPrint() + "]");
}

std::string PJust::toString() {
  return ((this->hash).toString()
          + std::to_string(this->view)
          + (this->auth).toString()
          + (this->auths).toString());
}


bool PJust::operator<(const PJust& s) const {
  return (auth < s.auth);
}
