#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "FData.h"


Hash   FData::getJusth()  { return (this->justh); }
View   FData::getJustv()  { return (this->justv); }
View   FData::getView()   { return (this->view);  }


void FData::serialize(salticidae::DataStream &data) const {
  data << this->justh << this->justv << this->view;
}


void FData::unserialize(salticidae::DataStream &data) {
  data >> this->justh >> this->justv >> this->view;
}


FData::FData(Hash justh, View justv, View view) {
  this->justh=justh;
  this->justv=justv;
  this->view=view;
}


FData::FData() {
  this->justh=Hash();
  this->justv=0;
  this->view=0;
}


FData::FData(salticidae::DataStream &data) {
  unserialize(data);
}



std::string FData::prettyPrint() {
  return ("FDATA[" + (this->justh).prettyPrint()
          + "," + std::to_string(this->justv)
          + "," + std::to_string(this->view)
          + "]");
}

std::string FData::toString() {
  return ((this->justh).toString() + std::to_string(this->justv)
          + std::to_string(this->view));
}


bool FData::operator==(const FData& s) const {
  if (DEBUG1) {
    std::cout << KYEL
              << "[1]" << (this->justh == s.justh)
              << "[2]" << (this->justv == s.justv)
              << "[3]" << (this->view == s.view)
              << KNRM << std::endl;
  }
  return (this->justh == s.justh
          && this->justv == s.justv
          && this->view == s.view);
}
