#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "Auth.h"


PID  Auth::getId()   { return this->id;   }
Hash Auth::getHash() { return this->hash; }


void Auth::serialize(salticidae::DataStream &data) const {
  data << this->id << this->hash;
}


void Auth::unserialize(salticidae::DataStream &data) {
  data >> this->id >> this->hash;
}


Auth::Auth() {}


Auth::Auth(bool b) {
  this->id=id;
  this->hash=Hash(b);
}


Auth::Auth(PID id, Hash hash) {
  this->id=id;
  this->hash=hash;
}


Auth::Auth(PID id, std::string secret, std::string text) {
  this->id=id;

  std::string t = std::to_string(id) + secret + text;

  if (!SHA256 ((const unsigned char *)t.c_str(), t.size(), this->hash.getHash())){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }
}


Auth::Auth(salticidae::DataStream &data) {
  unserialize(data);
}


bool Auth::verify(std::string secret, std::string text) {
  unsigned char h[SHA256_DIGEST_LENGTH];

  std::string t = std::to_string(this->id) + secret + text;

  if (!SHA256 ((const unsigned char *)t.c_str(), t.size(), h)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  return (this->hash == h);
}


std::string Auth::prettyPrint() {
  return (std::to_string(this->id) + "|" + this->hash.prettyPrint());
}


std::string Auth::toString() {
  std::string text;
  text = std::to_string(this->id);
  text += this->hash.toString();
  return text;
}

bool Auth::operator==(const Auth& s) const {
  return (id == s.id && hash == s.hash);
}

bool Auth::operator<(const Auth& s) const {
  return (id < s.id);
}
