#ifndef PROPOSAL_H
#define PROPOSAL_H


#include "config.h"
#include "types.h"
#include "Just.h"
#include "Block.h"

#include "salticidae/stream.h"


class Proposal {
 private:
  Just just;
  Block block;

 public:
  Proposal(Just just, Block block);
  Proposal();

  Just getJust();
  Block getBlock();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string prettyPrint();
  std::string toString();
};


#endif
