#include "VJust.h"

VJust::VJust() {
  this->set   = false;
  this->phase = PH2A;
  this->view  = 0;
  this->auth  = Auth();
}

VJust::VJust(Phase2 phase, View view, Auth auth) {
  this->set   = true;
  this->phase = phase;
  this->view  = view;
  this->auth  = auth;
}

VJust::VJust(bool b, Phase2 phase, View view, Auth auth) {
  this->set   = b;
  this->phase = phase;
  this->view  = view;
  this->auth  = auth;
}

bool   VJust::isSet()    { return this->set;   }
Phase2 VJust::getPhase() { return this->phase; }
View   VJust::getView()  { return this->view;  }
Auth   VJust::getAuth()  { return this->auth;  }


void VJust::serialize(salticidae::DataStream &data) const {
  data << this->set << this->phase << this->view << this->auth;
}


void VJust::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->phase >> this->view >> this->auth;
}


std::string VJust::prettyPrint() {
  return ("JUST[" + std::to_string(this->set)
          + "," + std::to_string(this->phase)
          + "," + std::to_string(this->view)
          + "," + (this->auth).prettyPrint() + "]");
}

std::string VJust::toString() {
  return (std::to_string(this->set)
          + std::to_string(this->phase)
          + std::to_string(this->view)
          + (this->auth).toString());
}
