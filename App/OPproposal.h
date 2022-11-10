#ifndef OPPROPOSAL_H
#define OPPROPOSAL_H


#include "config.h"
#include "types.h"
#include "Hash.h"
#include "Auth.h"

#include "salticidae/stream.h"


// OnePhase proposal when phase=OPpa and otherwise a vote
class OPproposal {
 private:
  Hash    hash;
  View    view;
  Auth    auth;

 public:
  OPproposal(Hash hase, View view, Auth auth);
  OPproposal();

  Hash    getHash();
  View    getView();
  Auth    getAuth();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
  std::string data();

  bool operator<(const OPproposal& s) const;
};


#endif
