#ifndef OPACCUM_H
#define OPACCUM_H


#include "Auth.h"
#include "Hash.h"

#include "salticidae/stream.h"


class OPaccum {

 private:
  bool set = false;
  NVkind kind;
  View view;
  Hash hash;
  unsigned int size;
  Auth auth;

 public:
  OPaccum();
  OPaccum(salticidae::DataStream &data);
  OPaccum(NVkind kind , View view, Hash hash, unsigned int size, Auth auth);
  OPaccum(bool set, NVkind kind , View view, Hash hash, unsigned int size, Auth auth);

  bool isSet();
  NVkind getKind();
  View getView();
  Hash getHash();
  unsigned int getSize();
  Auth getAuth();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
  std::string data();

  bool operator==(const OPaccum& s) const;
  bool operator<(const OPaccum& s) const;
};


#endif
