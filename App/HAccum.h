#ifndef HACCUM_H
#define HACCUM_H


#include "Auth.h"
#include "Hash.h"

#include "salticidae/stream.h"


class HAccum {

 private:
  bool set = false;
  View view;
  Hash preph;
  unsigned int size;
  Auth auth;
  Auth authp;

 public:
  HAccum();
  HAccum(salticidae::DataStream &data);
  HAccum(View view, Hash preph, unsigned int size, Auth auth, Auth authp);
  HAccum(bool set, View view, Hash preph, unsigned int size, Auth auth, Auth authp);

  bool isSet();
  View getView();
  Hash getPreph();
  unsigned int getSize();
  Auth getAuth();
  Auth getAuthp();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string data2string();

  std::string prettyPrint();
  std::string toString();

  bool operator==(const HAccum& s) const;
  bool operator<(const HAccum& s) const;
};


#endif
