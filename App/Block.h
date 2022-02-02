#ifndef BLOCK_H
#define BLOCK_H

#include <vector>

#include "config.h"
#include "types.h"
#include "Transaction.h"
#include "Hash.h"

#include "salticidae/stream.h"


class Block {
 private:
  bool set; // true if the block is not the dummy block
  Hash prevHash;
  unsigned int size = 0; // number of non-dummy transactions
  Transaction transactions[MAX_NUM_TRANSACTIONS];

  std::string transactions2string();

 public:
  Block(); // retruns the genesis block
  Block(bool b); // retruns the genesis block if b=true; and the dummy block otherwise
  //Block(Hash prevHash);
  Block(Hash prevHash, unsigned int size, Transaction transactions[MAX_NUM_TRANSACTIONS]); // creates an extension of 'block'

  bool extends(Hash h);
  Hash hash();

  bool isDummy(); // true if the block is not set
  unsigned int getSize();
  Transaction *getTransactions();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string toString();
  std::string prettyPrint();

  bool operator==(const Block& s) const;
};


#endif
