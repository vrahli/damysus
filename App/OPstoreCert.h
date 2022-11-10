#ifndef OPSTORECERT_H
#define OPSTORECERT_H


#include "config.h"
#include "types.h"
#include "Hash.h"
#include "Auths.h"
#include "OPstore.h"

#include "salticidae/stream.h"


class OPstoreCert {

 private:
  View  view;
  Hash  hash;
  View  v;
  Auths auths;

 public:
  OPstoreCert(View view, Hash hase, View v, Auths auths);
  OPstoreCert();

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

  bool operator<(const OPstoreCert& s) const;
};


#endif
