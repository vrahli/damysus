#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "HAccum.h"


bool         HAccum::isSet()    { return this->set;   }
View         HAccum::getView()  { return this->view;  }
Hash         HAccum::getPreph() { return this->preph; }
unsigned int HAccum::getSize()  { return this->size;  }
Auth         HAccum::getAuth()  { return this->auth;  }
Auth         HAccum::getAuthp() { return this->authp; }


void HAccum::serialize(salticidae::DataStream &data) const {
  data << this->set << this->view << this->preph << this->size << this->auth << this->authp;
}


void HAccum::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->view >> this->preph >> this->size >> this->auth >> this->authp;
}


HAccum::HAccum() {
//  this->auth  = Auth(false);
  this->authp = Auth(false);
}


HAccum::HAccum(salticidae::DataStream &data) {
  unserialize(data);
}


HAccum::HAccum(View view, Hash preph, unsigned int size, Auth auth, Auth authp) {
  this->set=true;
  this->view=view;
  this->preph=preph;
  this->size=size;
  this->auth=auth;
  this->authp=authp;
}


HAccum::HAccum(bool set, View view, Hash preph, unsigned int size, Auth auth, Auth authp) {
  this->set=set;
  this->view=view;
  this->preph=preph;
  this->size=size;
  this->auth=auth;
  this->authp=authp;
}


std::string HAccum::prettyPrint() {
  return ("ACCUM[" + std::to_string(this->set)
          + "," + std::to_string(this->view)
          + "," + this->preph.prettyPrint()
          + "," + std::to_string(this->size)
          + "," + this->auth.prettyPrint()
          + "," + this->authp.prettyPrint()
          + "]");
}

std::string HAccum::toString() {
  std::string text = std::to_string(this->set);
  text += std::to_string(this->view);
  text += this->preph.toString();
  text += std::to_string(this->size);
  text += this->auth.toString();
  text += this->authp.toString();
  return text;
}

std::string HAccum::data2string() {
  std::string text = std::to_string(this->set);
  text += std::to_string(this->view);
  text += this->preph.toString();
  text += std::to_string(this->size);
  return text;
}

bool HAccum::operator==(const HAccum& s) const {
  return (this->set == s.set
          && this->view == s.view
          && this->preph == s.preph
          && this->size == s.size
          && this->auth == s.auth
          && this->authp == s.authp);
}

bool HAccum::operator<(const HAccum& s) const {
  return (auth < s.auth);
}
