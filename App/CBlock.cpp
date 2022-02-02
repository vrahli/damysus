#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>


#include "CBlock.h"


void CBlock::serialize(salticidae::DataStream &data) const {
  data << this->set << this->executed << this->view << this->cert << this->size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    data << this->transactions[i];
  }
}


void CBlock::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->executed >> this->view >> this->cert >> this->size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    data >> this->transactions[i];
  }
}

std::string CBlock::transactions2string() {
  std::string s;
  for (int i = 0; i < this->size; i++) {
    s += this->transactions[i].toString();
  }
  return s;
}


std::string CBlock::toString() {
  std::string text = std::to_string(this->set)
//    + std::to_string(this->executed)
    + std::to_string(this->view)
    + this->cert.toString()
    + std::to_string(this->size)
    + transactions2string();
  return text;
}

std::string CBlock::prettyPrint() {
  std::string text = "";
  for (int i = 0; i < MAX_NUM_TRANSACTIONS && i < this->size; i++) {
    text += this->transactions[i].prettyPrint();
  }
  return ("JBLOCK[" + std::to_string(this->set)
          + "," + std::to_string(this->executed)
          + "," + std::to_string(this->view)
          + "," + this->cert.prettyPrint()
          + "," + std::to_string(this->size)
          + ",{" + text + "}]");
}


Hash CBlock::hash() {
  unsigned char h[SHA256_DIGEST_LENGTH];
  std::string text = toString();
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { h[i] = '0'; }

  //if (DEBUG0) { std::cout << KCYN << "hashing:" << text << KNRM << std::endl; }

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), h)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }
  return Hash(h);
}



// checks whether this block extends the argument -- a direct extension
bool CBlock::extends(Hash h) {
  return (this->cert.getHash() == h);
}


bool CBlock::isDummy() { return !this->set; }
bool CBlock::isSet() { return this->set; }
bool CBlock::isExecuted() { return this->executed; }
View CBlock::getView() { return this->view; }
unsigned int CBlock::getSize() { return this->size; }
CA CBlock::getCert() { return this->cert; }
Transaction *CBlock::getTransactions() { return this->transactions; }


void CBlock::markExecuted() {
  this->executed=true;
}


CBlock::CBlock(bool b) {
  this->set=b;
//  this->cert=CA();
  this->cert.tag = CERT;
  this->cert.cert = Cert(0,Hash(false),Signs());
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) { this->transactions[i] = Transaction(); }
}

CBlock::CBlock() {
  this->set=true;
//  this->cert=CA();
  this->cert.tag = CERT;
  this->cert.cert = Cert(0,Hash(false),Signs());
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) { this->transactions[i] = Transaction(); }
}

CBlock::CBlock(View view, CA cert, unsigned int size, Transaction transactions[MAX_NUM_TRANSACTIONS]) {
  this->set=true;
  this->view=view;
  this->cert=cert;
  this->size=size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    this->transactions[i]=transactions[i];
  }
}
