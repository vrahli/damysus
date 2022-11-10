#ifndef FVJUST_H
#define FVJUST_H


#include "Auths.h"
#include "FData.h"

#include "salticidae/stream.h"


// justrifications returned by TEEstore returned by TEEstore
class FVJust {

 private:
  bool set = false;
  FData data;
  Auth auth1; // This authenticates data
  Auth auth2; // this authenticates only the 2nd view in data (+ PH2_B)

 public:
  FVJust();
  FVJust(FData data, Auth auth1, Auth auth2);
  FVJust(bool set, FData data, Auth auth1, Auth auth2);

  bool  isSet();
  FData getData();
  Auth  getAuth1();
  Auth  getAuth2();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
};


#endif
