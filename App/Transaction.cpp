#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Transaction.h"


Transaction::Transaction() {
  this->clientid=0;
  this->transid=0;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data[i] = '0'; }
}

Transaction::Transaction(CID clientid, TID transid) {
  this->clientid=clientid;
  this->transid=transid;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data[i] = '0'; }
}

Transaction::Transaction(CID clientid, TID transid, char c) {
  this->clientid=clientid;
  this->transid=transid;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data[i] = c; }
}


CID Transaction::getCid() { return this->clientid; }
TID Transaction::getTid() { return this->transid;  }
unsigned char* Transaction::getData() { return this->data; }

void Transaction::serialize(salticidae::DataStream &data) const {
  data << this->clientid << this->transid;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { data << this->data[i]; }
}


void Transaction::unserialize(salticidae::DataStream &data) {
  data >> this->clientid >> this->transid;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { data >> this->data[i]; }
}


Transaction::Transaction(salticidae::DataStream &data) {
  unserialize(data);
}


std::string Transaction::toString() {
  std::string s = std::to_string(this->clientid) + std::to_string(this->transid);
  for (int i = 0; i < PAYLOAD_SIZE; i++) { s += std::to_string((this->data)[i]); }
  return s;
}


std::string Transaction::prettyPrint() {
  return ("TR[" + std::to_string(this->clientid) + "," + std::to_string(this->transid) + ",-]");
}


bool Transaction::operator<(const Transaction& s) const {
  return (clientid < s.clientid || transid < s.transid || data < s.data);
}

bool Transaction::operator==(const Transaction& s) const {
  if (this->clientid != s.clientid || this->transid !=  s.transid) { return false; }
  for (int i = 0; i < PAYLOAD_SIZE; i++) { if (!(data[i] == s.data[i])) { return false; } }
  return true;
}
