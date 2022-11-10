#ifndef FDATA_H
#define FDATA_H


#include "config.h"
#include "types.h"
#include "Hash.h"


#include "salticidae/stream.h"


class FData {
 private:
  Hash   justh;
  View   justv = 0;
  View   view = 0;

 public:
  FData(Hash justh, View justv, View view);
  FData(salticidae::DataStream &data);
  FData();

  Hash   getJusth();
  View   getJustv();
  View   getView();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  bool operator==(const FData& s) const;
};


#endif
