#ifndef CDATA_H
#define CDATA_H


#include "config.h"
#include "types.h"

#include "salticidae/stream.h"


// B: Block or Hash
// C: Cert or Acc or Void
template <class B, class C>
class CData { // Certified Data
 private:
  Phase1 phase;
  View view;
  B block; // a block or a hash
  C cert;

 public:
  CData(Phase1 phase, View view, B block, C cert);
  CData(salticidae::DataStream &data);
  CData();

  Phase1 getPhase();
  View   getView();
  B      getBlock();
  C      getCert();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();

  bool operator==(const CData<B,C>& s) const;
};


#endif
