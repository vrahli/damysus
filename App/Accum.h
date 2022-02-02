#ifndef ACCUM_H
#define ACCUM_H


#include "Signs.h"
#include "Hash.h"

#include "salticidae/stream.h"


class Accum {

 private:
  bool set = false;
  View view;
  View prepv;
  Hash preph;
  unsigned int size;
  Sign sign;

 public:
  Accum();
  Accum(salticidae::DataStream &data);
  Accum(View view, View prepv, Hash preph, unsigned int size, Sign sign);
  Accum(bool set, View view, View prepv, Hash preph, unsigned int size, Sign sign);

  bool isSet();
  View getView();
  View getPrepv();
  Hash getPreph();
  unsigned int getSize();
  Sign getSign();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string data2string();

  std::string prettyPrint();
  std::string toString();

  bool operator==(const Accum& s) const;
};


#endif
