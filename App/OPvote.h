#ifndef OPVOTE_H
#define OPVOTE_H


#include "config.h"
#include "types.h"
#include "Hash.h"
#include "Auths.h"

#include "salticidae/stream.h"


// OnePhase vote certificate
class OPvote {
 private:
  Hash  hash;
  View  view;
  Auths auths;

 public:
  OPvote(Hash hash, View view, Auths auths);
  OPvote();

  Hash  getHash();
  View  getView();
  Auths getAuths();

  void insert(Auths auths);

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
  std::string data();

  bool operator<(const OPvote& s) const;
};


#endif
