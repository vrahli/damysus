#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "Proposal.h"


Just Proposal::getJust()   { return (this->just);  }
Block Proposal::getBlock() { return (this->block); }

void Proposal::serialize(salticidae::DataStream &data) const {
  data << this->just << this->block;
}


void Proposal::unserialize(salticidae::DataStream &data) {
  data >> this->just >> this->block;
}


Proposal::Proposal(Just just, Block block) {
  this->just=just;
  this->block=block;
}


Proposal::Proposal() {}


std::string Proposal::prettyPrint() {
  return ("PROP[" + (this->just).prettyPrint() + "," + (this->block).prettyPrint() + "]");
}

std::string Proposal::toString() {
  return ((this->just).toString() + (this->block).toString());
}
