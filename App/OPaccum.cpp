#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "OPaccum.h"


bool         OPaccum::isSet()    { return this->set;  }
NVkind       OPaccum::getKind()  { return this->kind; }
View         OPaccum::getView()  { return this->view; }
Hash         OPaccum::getHash()  { return this->hash; }
unsigned int OPaccum::getSize()  { return this->size; }
Auth         OPaccum::getAuth()  { return this->auth; }


void OPaccum::serialize(salticidae::DataStream &data) const {
  data << this->set << this->kind << this->view << this->hash << this->size << this->auth;
}


void OPaccum::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->kind >> this->view >> this->hash >> this->size >> this->auth;
}


OPaccum::OPaccum() {}


OPaccum::OPaccum(salticidae::DataStream &data) {
  unserialize(data);
}


OPaccum::OPaccum(NVkind kind, View view, Hash hash, unsigned int size, Auth auth) {
  this->set=true;
  this->kind=kind;
  this->view=view;
  this->hash=hash;
  this->size=size;
  this->auth=auth;
}


OPaccum::OPaccum(bool set, NVkind kind, View view, Hash hash, unsigned int size, Auth auth) {
  this->set=set;
  this->kind=kind;
  this->view=view;
  this->hash=hash;
  this->size=size;
  this->auth=auth;
}


std::string OPaccum::prettyPrint() {
  return ("ACCUM[" + std::to_string(this->set)
          + "," + std::to_string(this->kind)
          + "," + std::to_string(this->view)
          + "," + this->hash.prettyPrint()
          + "," + std::to_string(this->size)
          + "," + this->auth.prettyPrint()
          + "]");
}

std::string OPaccum::toString() {
  std::string text = std::to_string(this->set);
  text += std::to_string(this->kind);
  text += std::to_string(this->view);
  text += this->hash.toString();
  text += std::to_string(this->size);
  text += this->auth.toString();
  return text;
}

std::string OPaccum::data() {
  std::string text = std::to_string(this->set);
  text += std::to_string(this->kind);
  text += std::to_string(this->view);
  text += this->hash.toString();
  text += std::to_string(this->size);
  return text;
}

bool OPaccum::operator==(const OPaccum& s) const {
  return (this->set == s.set
          && this->kind == s.kind
          && this->view == s.view
          && this->hash == s.hash
          && this->size == s.size
          && this->auth == s.auth);
}

bool OPaccum::operator<(const OPaccum& s) const {
  return (auth < s.auth);
}
