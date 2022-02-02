#include "Just.h"

Just::Just() {
  this->set   = false;
  this->rdata = RData();
  this->signs = Signs();
}

Just::Just(RData rdata, Sign sign) {
  this->set   = true;
  this->rdata = rdata;
  this->signs = Signs(sign);
}

Just::Just(RData rdata, Signs signs) {
  this->set   = true;
  this->rdata = rdata;
  this->signs = signs;
}

Just::Just(bool b, RData rdata, Signs signs) {
  this->set   = b;
  this->rdata = rdata;
  this->signs = signs;
}

bool  Just::isSet()    { return this->set;   }
RData Just::getRData() { return this->rdata; }
Signs Just::getSigns() { return this->signs; }


void Just::serialize(salticidae::DataStream &data) const {
  data << this->set << this->rdata << this->signs;
}


void Just::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->rdata >> this->signs;
}


std::string Just::prettyPrint() {
  return ("JUST[" + std::to_string(this->set) + "," + (this->rdata).prettyPrint() + "," + (this->signs).prettyPrint() + "]");
}

std::string Just::toString() {
  return (std::to_string(this->set) + (this->rdata).toString() + (this->signs).toString());
}


// The initial justification
bool Just::wellFormedInit() {
  return (this->rdata.getPropv() == 0
          && this->rdata.getJustv() == 0
          && this->rdata.getPhase() == PH1_NEWVIEW
          && this->signs.getSize() == 0);
}


bool Just::wellFormedNv() {
  return (this->rdata.getJusth().getSet()
          && this->rdata.getPhase() == PH1_NEWVIEW
          && this->signs.getSize() == 1);
}


bool Just::wellFormedPrep(unsigned int qsize) {
  return (this->rdata.getProph().getSet()
          && this->rdata.getPhase() == PH1_PREPARE
          && this->signs.getSize() == qsize);
}


// In the chained version there are 2 kinds of justifications
// (those created out of prepare messages, and those sent in new-view messages)
bool Just::wellFormed(unsigned int qsize) {
  return (wellFormedInit() || wellFormedNv() || wellFormedPrep(qsize));
}


// The view at which the certificate was generated depends on the kind of certificate we have
View Just::getCView() {
  if (wellFormedInit() || wellFormedNv()) { return rdata.getJustv(); }
  // othersize it must be wellFormedPrep(qsize)
  return rdata.getPropv();
}


Hash Just::getCHash() {
  if (wellFormedInit() || wellFormedNv()) { return rdata.getJusth(); }
  // otherwise it must be wellFormedPred(qsize)
  return rdata.getProph();
}
