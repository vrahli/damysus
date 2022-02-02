#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "Void.h"


void Void::serialize(salticidae::DataStream &data) const {}


void Void::unserialize(salticidae::DataStream &data) {}


Void::Void() {}


Void::Void(salticidae::DataStream &data) {
  unserialize(data);
}



std::string Void::prettyPrint() {
  return ("VOID");
}

std::string Void::toString() {
  std::string text = "";
  return text;
}


bool Void::operator==(const Void& s) const {
  return true;
}
