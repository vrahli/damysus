#ifndef PJUST_H
#define PJUST_H


#include "Auths.h"
#include "FData.h"

#include "salticidae/stream.h"


// a prepare certificate
class PJust {

 private:
  Hash hash;
  View view;
  Auth auth;
  Auths auths;

 public:
  PJust();
  PJust(bool b);
  PJust(Hash hash, View view, Auth auth, Auths auths);

  Hash  getHash();
  View  getView();
  Auth  getAuth();
  Auths getAuths();

  void add(Auth a);
  unsigned int sizeAuth();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  bool operator<(const PJust& s) const;
};


#endif
