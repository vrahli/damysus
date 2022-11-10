#ifndef OPSTORE_H
#define OPSTORE_H


#include "config.h"
#include "types.h"
#include "Hash.h"
#include "Auth.h"

#include "salticidae/stream.h"


// OnePhase store certificate
class OPstore {
 private:
  View view;
  Hash hash;
  View v;
  Auth auth;

 public:
  OPstore(View view, Hash hase, View v, Auth auth);
  OPstore();

  View getView();
  Hash getHash();
  View getV();
  Auth getAuth();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
  std::string data();

  bool operator<(const OPstore& s) const;
  bool operator==(const OPstore& s) const;
};


#endif
