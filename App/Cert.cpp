#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Cert.h"


void Cert::serialize(salticidae::DataStream &data) const {
  data << this->view << this->hash << this->signs;
}


void Cert::unserialize(salticidae::DataStream &data) {
  data >> this->view >> this->hash >> this->signs;
}


Cert::Cert() {}


Cert::Cert(salticidae::DataStream &data) {
  unserialize(data);
}


Cert::Cert(View view, Hash hash, Signs signs) {
  this->view=view;
  this->hash=hash;
  this->signs=signs;
}

View  Cert::getView()  { return this->view;  }
Hash  Cert::getHash()  { return this->hash;  }
Signs Cert::getSigns() { return this->signs; }


std::string Cert::prettyPrint() {
  return ("CERT[" + std::to_string(this->view)
          + "," + this->hash.prettyPrint()
          + "," + this->signs.prettyPrint()
          + "]");
}

std::string Cert::toString() {
  std::string text = std::to_string(this->view);
  text += this->hash.toString();
  text += this->signs.toString();
  return text;
}

bool Cert::operator==(const Cert& s) const {
  return (this->view == s.view && this->hash == s.hash && this->signs == s.signs);
}
