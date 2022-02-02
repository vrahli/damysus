#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>


#include "JBlock.h"


void JBlock::serialize(salticidae::DataStream &data) const {
  data << this->set << this->executed << this->view << this->just << this->size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    data << this->transactions[i];
  }
}


void JBlock::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->executed >> this->view >> this->just >> this->size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    data >> this->transactions[i];
  }
}

std::string JBlock::transactions2string() {
  std::string s;
  for (int i = 0; i < this->size; i++) {
    s += this->transactions[i].toString();
  }
  return s;
}


std::string JBlock::toString() {
  std::string text = std::to_string(this->set)
//    + std::to_string(this->executed)
    + std::to_string(this->view)
    + this->just.toString()
    + std::to_string(this->size)
    + transactions2string();
  return text;
}

std::string JBlock::prettyPrint() {
  std::string text = "";
  for (int i = 0; i < MAX_NUM_TRANSACTIONS && i < this->size; i++) {
    text += this->transactions[i].prettyPrint();
  }
  return ("JBLOCK[" + std::to_string(this->set)
          + "," + std::to_string(this->executed)
          + "," + std::to_string(this->view)
          + "," + this->just.prettyPrint()
          + "," + std::to_string(this->size)
          + ",{" + text + "}]");
}


Hash JBlock::hash() {
  unsigned char h[SHA256_DIGEST_LENGTH];
  std::string text = toString();

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), h)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }
  return Hash(h);
}



// checks whether this block extends the argument -- a direct extension
bool JBlock::extends(Hash h) {
  RData rdata = this->just.getRData();
  if (rdata.getProph().getSet()) { return (rdata.getProph() == h); }
  if (rdata.getJusth().getSet()) { return (rdata.getJusth() == h); }
  return false;
}


bool JBlock::isDummy() { return !this->set; }
bool JBlock::isSet() { return this->set; }
bool JBlock::isExecuted() { return this->executed; }
View JBlock::getView() { return this->view; }
unsigned int JBlock::getSize() { return this->size; }
Just JBlock::getJust() { return this->just; }
Transaction *JBlock::getTransactions() { return this->transactions; }


void JBlock::markExecuted() {
  this->executed=true;
}


JBlock::JBlock(bool b) {
  this->just=Just();
  this->set=b;
}

JBlock::JBlock() {
  this->just=Just();
  this->set=true;
}

JBlock::JBlock(View view, Just just, unsigned int size, Transaction transactions[MAX_NUM_TRANSACTIONS]) {
  this->set=true;
  this->view=view;
  this->just=just;
  this->size=size;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) {
    this->transactions[i]=transactions[i];
  }
}
