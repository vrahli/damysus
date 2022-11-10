#ifndef OPPREPARE_H
#define OPPREPARE_H


#include "Auths.h"
#include "OPstore.h"

#include "salticidae/stream.h"


// a OnePhase prepare certificate
class OPprepare {

 private:
  View view; // View at which the certifiate was created
  Hash hash;
  View v; // view at which the hash was proposed
  Auths auths;

 public:
  OPprepare();
  OPprepare(View view, Hash hash, View v, Auths auths);

  View  getView();
  Hash  getHash();
  View  getV();
  Auths getAuths();

  void insert(OPstore store);

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
  std::string data();

  bool operator<(const OPprepare& s) const;
};


#endif
