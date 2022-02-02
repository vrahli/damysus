#ifndef CBLOCK_H
#define CBLOCK_H

#include <vector>
#include <stdint.h>

#include "config.h"
#include "types.h"
#include "Transaction.h"
#include "Hash.h"
#include "Cert.h"
#include "Accum.h"

#include "salticidae/stream.h"


struct CA {
  CAtag tag;
  union{ Cert cert; Accum accum; };
  // Constructors
  CA() {
    tag = CERT;
    cert = Cert(0,Hash(false),Signs());
  }
  // Printing
  std::string toString() {
    if (tag == CERT) { return cert.toString(); }
    return accum.toString();
  }
  std::string prettyPrint() {
    if (tag == CERT) { return cert.prettyPrint(); }
    return accum.prettyPrint();
  }
  // Getters
  Hash getHash() {
    if (tag == CERT) { return cert.getHash(); }
    return accum.getPreph();
  }
  View getView() {
    if (tag == CERT) { return cert.getView(); }
    return accum.getPrepv();
  }
  View getCView() {
    if (tag == CERT) { return cert.getView(); }
    return accum.getView();
  }
  Signs getSigns() {
    if (tag == CERT) { return cert.getSigns(); }
    return Signs(accum.getSign());
  }
  bool isSet() {
    if (tag == CERT) { return cert.getHash().getSet(); }
    else return accum.isSet();
  }
  // Setters
  void setAccum(Accum a) {
    tag = ACCUM;
    accum = a;
  }
  void setCert(Cert c) {
    tag = CERT;
    cert = c;
  }
  // Serialization
  void serialize(salticidae::DataStream &data) const {
    data << tag;
    if (tag == CERT) { data << cert; }
    else { data << accum; }
  }
  void unserialize(salticidae::DataStream &data) {
    data >> tag;
    if (tag == CERT) { data >> cert; }
    else { data >> accum; }
  }
};



// Certified blocks
class CBlock {
 private:
  bool set;              // true if the block is not the dummy block (or blank block)
  bool executed = false; // We trun this to true once a block is executed
  View view = 0;         // This is the view at which the block is created -- if a block has a justification it will have a view view-1, but the block corresponding to this justification might have a lower view (capturing blank blocks)
  CA cert;             // justification for this block (points to a block)
  unsigned int size = 0; // number of non-dummy transactions
  Transaction transactions[MAX_NUM_TRANSACTIONS];

  std::string transactions2string();

 public:
  CBlock(); // retruns the genesis block
  CBlock(bool b); // retruns the genesis block if b=true; and the dummy block otherwise
  CBlock(View view, CA cert, unsigned int size, Transaction transactions[MAX_NUM_TRANSACTIONS]); // creates an extension of 'block'

  bool extends(Hash h);
  Hash hash();

  bool isDummy(); // true if the block is not set
  bool isSet(); // same as !isDummy()
  bool isExecuted();
  View getView();
  unsigned int getSize();
  CA getCert();
  Transaction *getTransactions();

  void markExecuted();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);

  std::string toString();
  std::string prettyPrint();
};


#endif
