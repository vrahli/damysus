#ifndef FJUST_H
#define FJUST_H


#include "Auths.h"
#include "FData.h"

#include "salticidae/stream.h"


// a new-view certificate
class FJust {

 private:
  bool set = false;
  FData data; // data
  Auth auth;

 public:
  FJust();
  FJust(FData data, Auth auth);
  FJust(bool b, FData data, Auth auth);

  bool  isSet();
  FData getData();
  Auth  getAuth();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  View getCView();
  Hash getCHash();
};


#endif
