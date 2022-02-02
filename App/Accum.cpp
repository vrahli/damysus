#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Accum.h"


bool         Accum::isSet()    { return this->set;   }
View         Accum::getView()  { return this->view;  }
View         Accum::getPrepv() { return this->prepv; }
Hash         Accum::getPreph() { return this->preph; }
unsigned int Accum::getSize()  { return this->size;  }
Sign         Accum::getSign()  { return this->sign;  }

void Accum::serialize(salticidae::DataStream &data) const {
  data << this->set << this->view << this->prepv << this->preph << this->size << this->sign;
}


void Accum::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->view >> this->prepv >> this->preph >> this->size >> this->sign;
}


Accum::Accum() {}


Accum::Accum(salticidae::DataStream &data) {
  unserialize(data);
}


Accum::Accum(View view, View prepv, Hash preph, unsigned int size, Sign sign) {
  this->set=true;
  this->view=view;
  this->prepv=prepv;
  this->preph=preph;
  this->size=size;
  this->sign=sign;
}


Accum::Accum(bool set, View view, View prepv, Hash preph, unsigned int size, Sign sign) {
  this->set=set;
  this->view=view;
  this->prepv=prepv;
  this->preph=preph;
  this->size=size;
  this->sign=sign;
}


std::string Accum::prettyPrint() {
  return ("ACCUM[" + std::to_string(this->set)
          + "," + std::to_string(this->view)
          + "," + std::to_string(this->prepv)
          + "," + this->preph.prettyPrint()
          + "," + std::to_string(this->size)
          + "," + this->sign.prettyPrint()
          + "]");
}

std::string Accum::toString() {
  std::string text = std::to_string(this->set);
  text += std::to_string(this->view);
  text += std::to_string(this->prepv);
  text += this->preph.toString();
  text += std::to_string(this->size);
  text += this->sign.toString();
  return text;
}

std::string Accum::data2string() {
  std::string text = std::to_string(this->set);
  text += std::to_string(this->view);
  text += std::to_string(this->prepv);
  text += this->preph.toString();
  text += std::to_string(this->size);
  return text;
}

bool Accum::operator==(const Accum& s) const {
  return (this->set == s.set
          && this->view == s.view
          && this->prepv == s.prepv
          && this->preph == s.preph
          && this->size == s.size
          && this->sign == s.sign);
}
