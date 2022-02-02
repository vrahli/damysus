#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "Hash.h"


bool Hash::getSet()  { return this->set;  }
unsigned char * Hash::getHash() { return this->hash; }
bool Hash::isDummy() { return !this->set; }

void Hash::serialize(salticidae::DataStream &data) const {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { data << this->hash[i]; }
  data << this->set;
}


void Hash::unserialize(salticidae::DataStream &data) {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { data >> this->hash[i]; }
  data >> this->set;
}


Hash::Hash() {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { this->hash[i] = '0'; }
  this->set=true;
}

Hash::Hash(bool b) {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { this->hash[i] = '0'; }
  this->set=b;
}


Hash::Hash(unsigned char hash[SHA256_DIGEST_LENGTH]) {
  memcpy(this->hash,hash,SHA256_DIGEST_LENGTH);
  this->set=true;
}

Hash::Hash(bool b, unsigned char hash[SHA256_DIGEST_LENGTH]) {
  memcpy(this->hash,hash,SHA256_DIGEST_LENGTH);
  this->set=b;
}


Hash::Hash(salticidae::DataStream &data) {
  unserialize(data);
  //this->set=true;
}


bool Hash::isZero() {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { if (this->hash[i] != '0') { return false; } }
  return true;
}

std::string Hash::prettyPrint() {
  return ("#" + std::to_string(this->set));
}

std::string Hash::toString() {
  std::string text;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { text += this->hash[i]; }
  text += std::to_string(this->set);
  return text;
}

bool Hash::operator==(const Hash& s) const {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { if (hash[i] != s.hash[i]) { return false; } }
  return true;
}
