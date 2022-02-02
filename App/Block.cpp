#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>


#include "Block.h"


void Block::serialize(salticidae::DataStream &data) const {
  data << this->set << this->prevHash << this->size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    data << this->transactions[i];
  }
}


void Block::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->prevHash >> this->size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    data >> this->transactions[i];
  }
}

std::string Block::transactions2string() {
  std::string s;
  for (int i = 0; i < this->size; i++) {
    s += this->transactions[i].toString();
  }
  return s;
}


std::string Block::toString() {
  std::string text = std::to_string(this->set)
    + this->prevHash.toString()
    + std::to_string(this->size)
    + transactions2string();
  return text;
}

std::string Block::prettyPrint() {
  std::string text = "";
  for (int i = 0; i < MAX_NUM_TRANSACTIONS && i < this->size; i++) {
    text += this->transactions[i].prettyPrint();
  }
  return ("BLOCK[" + std::to_string(this->set)
          + "," + this->prevHash.prettyPrint()
          + "," + std::to_string(this->size)
          + ",{" + text + "}]");
}


Hash Block::hash() {
  unsigned char h[SHA256_DIGEST_LENGTH];
  std::string text = toString();

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), h)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }
  return Hash(h);
}


// checks whether this block extends the argument
bool Block::extends(Hash h) {
  return (this->prevHash == h);
}

bool Block::isDummy() { return !this->set; }
unsigned int Block::getSize() { return this->size; }
Transaction *Block::getTransactions() { return this->transactions; }


Block::Block(bool b) {
  this->prevHash=Hash();
  this->set=b;
}

Block::Block() {
  this->prevHash=Hash();
  this->set=true;
}

/*Block::Block(Hash prevHash) {
  this->prevHash=prevHash;
  this->set=true;
}*/

Block::Block(Hash prevHash, unsigned int size, Transaction transactions[MAX_NUM_TRANSACTIONS]) {
  this->set=true;
  this->prevHash=prevHash;
  this->size=size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    this->transactions[i]=transactions[i];
  }
}

bool Block::operator==(const Block& s) const {
  if (this->set != s.set || !(this->prevHash ==  s.prevHash) || this->size != s.size) { return false; }
  for (int i = 0; i < MAX_NUM_TRANSACTIONS && i < this->size; i++) { if (!(transactions[i] == s.transactions[i])) { return false; } }
  return true;
}
