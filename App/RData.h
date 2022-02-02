#ifndef RDATA_H
#define RDATA_H


#include "config.h"
#include "types.h"
#include "Hash.h"


#include "salticidae/stream.h"


// Round Data
class RData {
 private:
  Hash   proph;
  View   propv = 0;
  Hash   justh;
  View   justv = 0;
  Phase1 phase;

 public:
  RData(Hash proph, View propv, Hash justh, View justv, Phase1 phase);
  RData(salticidae::DataStream &data);
  RData();

  Hash   getProph();
  View   getPropv();
  Hash   getJusth();
  View   getJustv();
  Phase1 getPhase();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  bool operator==(const RData& s) const;
};


#endif
