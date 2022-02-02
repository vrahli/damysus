#ifndef JUST_H
#define JUST_H


#include "Signs.h"
#include "RData.h"

#include "salticidae/stream.h"


class Just {

 private:
  bool set = false;
  RData rdata; // round data
  Signs signs; // signature

 public:
  Just();
  Just(RData rdata, Sign sign);
  Just(RData rdata, Signs signs);
  Just(bool b, RData rdata, Signs signs);

  bool  isSet();
  RData getRData();
  Signs getSigns();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  bool wellFormedInit();
  bool wellFormedNv();
  bool wellFormedPrep(unsigned int qsize);
  bool wellFormed(unsigned int qsize);
  View getCView();
  Hash getCHash();
};


#endif
