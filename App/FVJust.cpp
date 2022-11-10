#include "FVJust.h"

FVJust::FVJust() {
  this->set   = false;
  this->data  = FData();
  this->auth1 = Auth();
  this->auth2 = Auth();
}

FVJust::FVJust(FData data, Auth auth1, Auth auth2) {
  this->set   = true;
  this->data  = data;
  this->auth1 = auth1;
  this->auth2 = auth2;
}

FVJust::FVJust(bool set, FData data, Auth auth1, Auth auth2) {
  this->set   = set;
  this->data  = data;
  this->auth1 = auth1;
  this->auth2 = auth2;
}

bool  FVJust::isSet()    { return this->set;   }
FData FVJust::getData()  { return this->data;  }
Auth  FVJust::getAuth1() { return this->auth1; }
Auth  FVJust::getAuth2() { return this->auth2; }


void FVJust::serialize(salticidae::DataStream &data) const {
  data << this->set << this->data << this->auth1 << this->auth2;
}


void FVJust::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->data >> this->auth1 >> this->auth2;
}


std::string FVJust::prettyPrint() {
  return ("JUST[" + std::to_string(this->set)
          + "," + (this->data).prettyPrint()
          + "," + (this->auth1).prettyPrint()
          + "," + (this->auth2).prettyPrint()
          + "]");
}

std::string FVJust::toString() {
  return (std::to_string(this->set)
          + (this->data).toString()
          + (this->auth1).toString()
          + (this->auth2).toString());
}
