#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Signs.h"


void Signs::serialize(salticidae::DataStream &data) const {
  data << this->size;
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) {
    data << this->signs[i];
  }
}


void Signs::unserialize(salticidae::DataStream &data) {
  data >> this->size;
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) {
    data >> this->signs[i];
  }
}



Signs::Signs(Sign sign) {
  this->signs[0] = sign;
  this->size=1;
}


Signs::Signs(unsigned int size, Sign signs[MAX_NUM_SIGNATURES]) {
  this->size=size;
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) {
    this->signs[i] = signs[i];
  }
}


Signs::Signs(salticidae::DataStream &data) {
  unserialize(data);
}


Signs::Signs() {
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) { this->signs[i] = Sign(); }
}

unsigned int Signs::getSize() {
  return this->size;
}

Sign Signs::get(unsigned int n) {
  return this->signs[n];
}


bool Signs::verify(Stats &stats, PID id, Nodes nodes, std::string s) {
  //if (DEBUG1) { std::cout << KYEL << "size signs=" << this->size << KNRM << std::endl; }
  for (int i = 0; i < this->size; i++) {
    Sign sign = this->signs[i];
    PID node = sign.getSigner();
    //if (DEBUG1) { std::cout << KYEL << "node:" << node << KNRM << std::endl; }
    if (true) { //(id != node) { // we don't check our own signature
      NodeInfo *nfo = nodes.find(node);
      if (nfo) {
        // TODO: The id of the signer should also be added to the string (for signing and verifying---maybe in Sign.cpp instead?)
        auto start = std::chrono::steady_clock::now();
        bool b = sign.verify(nfo->getPub(),s);
        auto end = std::chrono::steady_clock::now();
        double time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
        stats.addCryptoVerifTime(time);
        if (!b) {
          if (DEBUG1) { std::cout << KYEL << "couldn't verify signature from " << nfo->getId() << KNRM << std::endl; }
          return false;
        }
      } else {
        if (DEBUG1) { std::cout << KYEL << "couldn't get info for " << node << KNRM << std::endl; }
        return false;
      }
    }
  }
  return true;
}


std::string Signs::prettyPrint() {
  std::string text = "";
  for (int i = 0; i < this->size; i++) {
    text += ":" + signs[i].prettyPrint();
  }
  return ("SIGNS[-" + std::to_string(this->size) + "-" + text + ":]");
}

std::string Signs::toString() {
  std::string text = std::to_string(this->size);
  for (int i = 0; i < this->size; i++) {
    text += signs[i].toString();
  }
  return text;
}


bool Signs::operator<(const Signs& s) const {
  if (size < s.size) { return true; }
  if (s.size < size) { return false; }
  // They must have the same acutal size
  for (int i = 0; i < size; i++) {
    if (signs[i] < s.signs[i]) { return true; }
    if (s.signs[i] < signs[i]) { return false; }
  }
  return false;
}

bool Signs::hasSigned(PID p) {
  for (int i = 0; i < this->size; i++) {
    if (this->signs[i].getSigner() == p) { return true; }
  }
  return false;
}

std::set<PID> Signs::getSigners() {
  std::set<PID> s;
  for (int i = 0; i < this->size; i++) {
    s.insert(this->signs[i].getSigner());
  }
  return s;
}

std::string Signs::printSigners() {
  std::string s = "-";
  for (int i = 0; i < this->size; i++) {
    s += this->signs[i].getSigner() + "-";
  }
  return s;
}

void Signs::add(Sign sign) {
  this->signs[this->size]=sign;
  this->size++;
}

void Signs::addUpto(Signs others, unsigned int n) {
  for (int i = 0; i < others.getSize() && this->size < n; i++) {
    this->add(others.get(i));
  }
}

bool Signs::operator==(const Signs& s) const {
  if (this->size != s.size) { return false; }
  for (int i = 0; i < MAX_NUM_SIGNATURES && i < this->size; i++) { if (!(signs[i] == s.signs[i])) { return false; } }
  return true;
}
