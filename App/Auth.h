#ifndef AUTH_H
#define AUTH_H


#include "config.h"
#include "types.h"
#include "Hash.h"

#include "salticidae/stream.h"


class Auth {
 private:
  PID id;
  Hash hash;

 public:
  Auth();
  Auth(bool b);
  Auth(PID id, Hash hash);
  Auth(salticidae::DataStream &data);
  Auth(PID id, std::string secret, std::string text);

  bool verify(std::string secret, std::string text);

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  PID getId();
  Hash getHash();

  std::string prettyPrint();
  std::string toString();

  bool operator==(const Auth& s) const;
  bool operator<(const Auth& s) const;
};


#endif
