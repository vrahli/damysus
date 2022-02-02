#ifndef VOTE_H
#define VOTE_H


#include "config.h"
#include "types.h"
#include "CData.h"
#include "Signs.h"

#include "salticidae/stream.h"


// B: Block or Hash
// C: Cert or Acc or Void
template <class B, class C>
class Vote { // Certified Data
 private:
  CData<B,C> cdata;
  Sign sign;

 public:
  Vote(CData<B,C> cdata, Sign sign);
  Vote(salticidae::DataStream &data);
  Vote();

  CData<B,C> getCData();
  Sign       getSign();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
};


#endif
