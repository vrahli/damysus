#ifndef JBLOCK_H
#define JBLOCK_H

#include <vector>

#include "config.h"
#include "types.h"
#include "Transaction.h"
#include "Hash.h"
#include "Just.h"

#include "salticidae/stream.h"


// Justified blocks
class JBlock {
 private:
  bool set;              // true if the block is not the dummy block (or blank block)
  bool executed = false; // We trun this to true once a block is executed
  View view = 0;         // This is the view at which the block is created -- if a block has a justification it will have a view view-1, but the block corresponding to this justification might have a lower view (capturing blank blocks)
  Just just;             // justification for this block (points to a block)
  unsigned int size = 0; // number of non-dummy transactions
  Transaction transactions[MAX_NUM_TRANSACTIONS];

  std::string transactions2string();

 public:
  JBlock(); // retruns the genesis block
  JBlock(bool b); // retruns the genesis block if b=true; and the dummy block otherwise
  JBlock(View view, Just just, unsigned int size, Transaction transactions[MAX_NUM_TRANSACTIONS]); // creates an extension of 'block'

  bool extends(Hash h);
  Hash hash();

  bool isDummy(); // true if the block is not set
  bool isSet(); // same as !isDummy()
  bool isExecuted();
  View getView();
  unsigned int getSize();
  Just getJust();
  Transaction *getTransactions();

  void markExecuted();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string toString();
  std::string prettyPrint();
};


#endif
