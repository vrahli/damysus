#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Auths.h"


void Auths::serialize(salticidae::DataStream &data) const {
  data << this->size;
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) {
    data << this->auths[i];
  }
}


void Auths::unserialize(salticidae::DataStream &data) {
  data >> this->size;
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) {
    data >> this->auths[i];
  }
}



Auths::Auths(Auth auth) {
  this->auths[0] = auth;
  this->size=1;
}


Auths::Auths(unsigned int size, Auth auths[MAX_NUM_SIGNATURES]) {
  this->size=size;
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) {
    this->auths[i] = auths[i];
  }
}


Auths::Auths(salticidae::DataStream &data) {
  unserialize(data);
}


Auths::Auths() {
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) { this->auths[i] = Auth(); }
}

unsigned int Auths::getSize() {
  return this->size;
}

Auth Auths::get(unsigned int n) {
  return this->auths[n];
}


bool Auths::verify(std::string secret, std::string s) {
  for (int i = 0; i < this->size; i++) {
    if (!this->auths[i].verify(secret,s)) {
      if (DEBUG1) { std::cout << KYEL << "couldn't verify signature from " << auths[i].getId() << KNRM << std::endl; }
      return false;
    }
  }
  return true;
}


std::string Auths::prettyPrint() {
  std::string text = "";
  for (int i = 0; i < this->size; i++) {
    text += ":" + auths[i].prettyPrint();
  }
  return ("AUTHS[-" + std::to_string(this->size) + "-" + text + ":]");
}

std::string Auths::toString() {
  std::string text = std::to_string(this->size);
  for (int i = 0; i < this->size; i++) {
    text += auths[i].toString();
  }
  return text;
}


bool Auths::operator<(const Auths& s) const {
  if (size < s.size) { return true; }
  if (s.size < size) { return false; }
  // They must have the same acutal size
  for (int i = 0; i < size; i++) {
    if (auths[i] < s.auths[i]) { return true; }
    if (s.auths[i] < auths[i]) { return false; }
  }
  return false;
}

bool Auths::hasSigned(PID p) {
  for (int i = 0; i < this->size; i++) {
    if (this->auths[i].getId() == p) { return true; }
  }
  return false;
}

std::set<PID> Auths::getSigners() {
  std::set<PID> s;
  for (int i = 0; i < this->size; i++) {
    s.insert(this->auths[i].getId());
  }
  return s;
}

std::string Auths::printSigners() {
  std::string s = "-";
  for (int i = 0; i < this->size; i++) {
    s += this->auths[i].getId() + "-";
  }
  return s;
}

void Auths::add(Auth auth) {
  if (this->size < MAX_NUM_SIGNATURES) {
    //if (DEBUG1) { std::cout << KYEL << "Auths:add:" << MAX_NUM_SIGNATURES << ":" << this->size << KNRM << std::endl; }
    this->auths[this->size]=auth;
    this->size++;
  }
}

void Auths::addUpto(Auths others, unsigned int n) {
  for (int i = 0; i < others.getSize() && this->size < n; i++) {
    this->add(others.get(i));
  }
}

bool Auths::in(PID id) {
  for (int i = 0; i < this->size; i++) {
    if (id == this->auths[i].getId()) { return true; }
  }
  return false;
}

void Auths::addNew(Auth auth) {
  if (!in(auth.getId())) { add(auth); }
}

void Auths::addNew(Auths others) {
  for (int i = 0; i < others.getSize(); i++) {
    this->addNew(others.get(i));
  }
}

bool Auths::operator==(const Auths& s) const {
  if (this->size != s.size) { return false; }
  for (int i = 0; i < MAX_NUM_SIGNATURES && i < this->size; i++) { if (!(auths[i] == s.auths[i])) { return false; } }
  return true;
}
