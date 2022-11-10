#include "FJust.h"

FJust::FJust() {
  this->set  = false;
  this->data = FData();
  this->auth = Auth();
}

FJust::FJust(FData data, Auth auth) {
  this->set  = true;
  this->data = data;
  this->auth = auth;
}

FJust::FJust(bool b, FData data, Auth auth) {
  this->set  = b;
  this->data = data;
  this->auth = auth;
}

bool  FJust::isSet()   { return this->set;  }
FData FJust::getData() { return this->data; }
Auth  FJust::getAuth() { return this->auth; }


void FJust::serialize(salticidae::DataStream &data) const {
  data << this->set << this->data << this->auth;
}


void FJust::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->data >> this->auth;
}


std::string FJust::prettyPrint() {
  return ("JUST[" + std::to_string(this->set)
          + "," + (this->data).prettyPrint()
          + "," + (this->auth).prettyPrint()
          + "]");
}

std::string FJust::toString() {
  return (std::to_string(this->set)
          + (this->data).toString()
          + (this->auth).toString());
}


View FJust::getCView() {
  return data.getJustv();
}


Hash FJust::getCHash() {
  return data.getJusth();
}
