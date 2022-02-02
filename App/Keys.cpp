#include <iostream>

#include "config.h"
#include "KeysFun.h"


int main(int argc, char const *argv[]) {
  // Geting inputs
  unsigned int myid = 0;
  KeysFun kf;

  if (argc > 1) { sscanf(argv[1], "%d", &myid); }
  std::cout << KMAG << "[my id is: " << myid << "]" << KNRM << std::endl;

  std::cout << KMAG << "--- generating RSA_4096 keys" << KNRM << std::endl;
  kf.generateRsa4096Keys(myid);

  std::cout << KMAG << "--- generating RSA_2048 keys" << KNRM << std::endl;
  kf.generateRsa2048Keys(myid);

  std::cout << KMAG << "--- generating EC_521 keys" << KNRM << std::endl;
  kf.generateEc521Keys(myid);

  std::cout << KMAG << "--- generating EC_256 keys" << KNRM << std::endl;
  kf.generateEc256Keys(myid);

  //std::cout << KMAG << "--- generating BLS keys" << KNRM << std::endl;
  //kf.generateBlsKeys(myid);
  return 0;
}
