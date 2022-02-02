#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "RData.h"


Hash   RData::getProph()  { return (this->proph); }
View   RData::getPropv()  { return (this->propv); }
Hash   RData::getJusth()  { return (this->justh); }
View   RData::getJustv()  { return (this->justv); }
Phase1 RData::getPhase()  { return (this->phase); }


void RData::serialize(salticidae::DataStream &data) const {
  data << this->proph << this->propv << this->justh << this->justv << this->phase;
}


void RData::unserialize(salticidae::DataStream &data) {
  data >> this->proph >> this->propv >> this->justh >> this->justv >> this->phase;
}


RData::RData(Hash proph, View propv, Hash justh, View justv, Phase1 phase) {
  this->proph=proph;
  this->propv=propv;
  this->justh=justh;
  this->justv=justv;
  this->phase=phase;
}


RData::RData() {
  this->proph=Hash();
  this->propv=0;
  this->justh=Hash();
  this->justv=0;
  this->phase=PH1_NEWVIEW;
}


RData::RData(salticidae::DataStream &data) {
  unserialize(data);
}


std::string phase2string(Phase1 phase) {
  if (phase == PH1_NEWVIEW) { return "PH_NEWVIEW"; }
  else if (phase == PH1_PREPARE) { return "PH_PREPARE"; }
  else if (phase == PH1_PRECOMMIT) { return "PH_PRECOMMIT"; }
  else if (phase == PH1_COMMIT) { return "PH_COMMIT"; }
  else { return ""; }
  return "";
}


std::string RData::prettyPrint() {
  return ("RDATA[" + (this->proph).prettyPrint() + "," + std::to_string(this->propv)
          + "," + (this->justh).prettyPrint() + "," + std::to_string(this->justv)
          + "," + phase2string(this->phase)
          + "]");
}

std::string RData::toString() {
  return ((this->proph).toString() + std::to_string(this->propv)
          + (this->justh).toString() + std::to_string(this->justv)
          + std::to_string(this->phase));
}


bool RData::operator==(const RData& s) const {
  if (DEBUG1) {
    std::cout << KYEL
              << "[1]" << (this->proph == s.proph)
              << "[2]" << (this->propv == s.propv)
              << "[3]" << (this->justh == s.justh)
              << "[4]" << (this->justv == s.justv)
              << "[5]" << (this->phase == s.phase)
              << KNRM << std::endl;
  }
  return (this->proph == s.proph
          && this->propv == s.propv
          && this->justh == s.justh
          && this->justv == s.justv
          && this->phase == s.phase);
}
