#ifndef CERT_H
#define CERT_H


#include "Signs.h"
#include "Hash.h"

#include "salticidae/stream.h"


class Cert {

 private:
  View view;
  Hash hash;
  Signs signs;

 public:
  Cert();
  Cert(salticidae::DataStream &data);
  Cert(View view, Hash hash, Signs signs);

  View getView();
  Hash getHash();
  Signs getSigns();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  bool operator==(const Cert& s) const;
};


#endif
