#ifndef VJUST_H
#define VJUST_H


#include "Auth.h"

#include "salticidae/stream.h"


//  a vote
class VJust {

 private:
  bool set;
  Phase2 phase;
  View view;
  Auth auth;

 public:
  VJust();
  VJust(Phase2 phase, View view, Auth auth);
  VJust(bool b, Phase2 phase, View view, Auth auth);

  bool isSet();
  Phase2 getPhase();
  View getView();
  Auth getAuth();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
};


#endif
