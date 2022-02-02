#ifndef SIGN_H
#define SIGN_H

//#include <ostream>
#include <iostream>
#include <array>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>

#include "types.h"
#include "config.h"
#include "key.h"

#include "salticidae/stream.h"


class Sign {

 private:
  bool set = false;
  PID signer; // the signer
  unsigned char sign[SIGN_LEN];

 public:
  Sign();
  Sign(PID signer, char c);
  Sign(PID signer, unsigned char s[SIGN_LEN]);
  Sign(bool b, PID signer, unsigned char s[SIGN_LEN]);
  Sign(KEY priv, PID signer, std::string text);

  unsigned char * getSign();
  PID getSigner();
  bool isSet();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string toString();
  std::string prettyPrint();

  bool verify(KEY pub, std::string text);

  //friend std::ostream& operator<<(std::ostream& os, const Sign &s);
  bool operator<(const Sign& s) const;
  bool operator==(const Sign& s) const;
};

#endif
