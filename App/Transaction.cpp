#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <algorithm>

#include "Transaction.h"


Transaction::Transaction() {
  this->clientid=0;
  this->transid=0;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data[i] = '0'; }
  //for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data.push_back('0'); }
}

Transaction::Transaction(CID clientid, TID transid) {
  this->clientid=clientid;
  this->transid=transid;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data[i] = '0'; }
  //for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data.push_back('0'); }
}

Transaction::Transaction(CID clientid, TID transid, char c) {
  this->clientid=clientid;
  this->transid=transid;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data[i] = c; }
  //for (int i = 0; i < PAYLOAD_SIZE; i++) { this->data.push_back(c); }
}


CID Transaction::getCid() { return this->clientid; }
TID Transaction::getTid() { return this->transid;  }
unsigned char* Transaction::getData() { return &(this->data[0]); }


std::string Transaction::toString() const {
  std::string s = std::to_string(this->clientid) + std::to_string(this->transid);
  std::string sd(this->data.begin(), this->data.end());
  //for (int i = 0; i < PAYLOAD_SIZE; i++) { s += std::to_string((this->data)[i]); }
  return s + sd;
}


void Transaction::serialize(salticidae::DataStream &data) const {
  data << this->clientid << this->transid;
  //for (int i = 0; i < PAYLOAD_SIZE; i++) { data << this->data[i]; }
  data.put_data(this->data);
}


void Transaction::unserialize(salticidae::DataStream &data) {
  data >> this->clientid >> this->transid;
  unsigned int n = PAYLOAD_SIZE * sizeof(uint8_t);
  const uint8_t *arr = data.get_data_inplace(n);
  std::copy_n(arr, n, std::begin(this->data));
  //for (int i = 0; i < PAYLOAD_SIZE; i++) { data >> this->data[i]; }
  //for (int i = 0; i < PAYLOAD_SIZE; i++) { uint8_t u; data >> u; this->data.push_back(u); }
}


Transaction::Transaction(salticidae::DataStream &data) {
  unserialize(data);
}


std::string Transaction::prettyPrint() const {
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


unsigned int Transaction::size() {
  return (sizeof(CID) + sizeof(TID) + (PAYLOAD_SIZE * sizeof(uint8_t)));
}
