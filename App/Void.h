#ifndef VOID_H
#define VOID_H


#include "config.h"
#include "types.h"

#include <string>

#include "salticidae/stream.h"


class Void {
 private:

 public:
  Void();
  Void(salticidae::DataStream &data);

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  bool operator==(const Void& s) const;
};


#endif
