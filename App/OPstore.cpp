#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "OPstore.h"


View OPstore::getView() { return (this->view); }
Hash OPstore::getHash() { return (this->hash);  }
View OPstore::getV()    { return (this->v);  }
Auth OPstore::getAuth() { return (this->auth);  }


void OPstore::serialize(salticidae::DataStream &data) const {
  data << this->view << this->hash << this->v << this->auth;
}


void OPstore::unserialize(salticidae::DataStream &data) {
  data >> this->view >> this->hash >> this->v >> this->auth;
}


OPstore::OPstore(View view, Hash hash, View v, Auth auth) {
  this->view=view;
  this->hash=hash;
  this->v=v;
  this->auth=auth;
}


OPstore::OPstore() {}


std::string OPstore::prettyPrint() {
  return ("STORE[" + std::to_string(this->view)
          + "," + (this->hash).prettyPrint()
          + "," + std::to_string(this->v)
          + "," + (this->auth).prettyPrint()
          + "]");
}

std::string OPstore::toString() {
  return (std::to_string(this->view)
          + (this->hash).toString()
          + std::to_string(this->v)
          + (this->auth).toString());
}

std::string OPstore::data() {
  return (std::to_string(this->view)
          + (this->hash).toString()
          + std::to_string(this->v));
}

bool OPstore::operator<(const OPstore& s) const {
  return (view < s.view || (view == s.view && auth < s.auth));
}


bool OPstore::operator==(const OPstore& s) const {
  return (view == s.view && hash == s.hash && v == s.v && auth == s.auth);
}
