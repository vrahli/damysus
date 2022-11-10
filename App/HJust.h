#ifndef HJUST_H
#define HJUST_H


#include "Auth.h"

#include "salticidae/stream.h"


// a prepare pre-certificate
class HJust {

 private:
  bool set;
  Hash hash;
  View view;
  Auth auth;

 public:
  HJust();
  HJust(Hash hash, View view, Auth auth);
  HJust(bool b, Hash hash, View view, Auth auth);

  bool isSet();
  Hash getHash();
  View getView();
  Auth getAuth();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
};


#endif
