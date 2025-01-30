#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

#include "config.h"
#include "types.h"

#include "salticidae/stream.h"


class Transaction {

 private:
  CID clientid;
  TID transid; // transaction id (0 is reserved for dummy transactions)
  //unsigned char data[PAYLOAD_SIZE];
  std::array<unsigned char,PAYLOAD_SIZE> data;
  //bytearray_t data;

 public:
  Transaction();
  Transaction(CID clientid, TID transid);
  Transaction(CID clientid, TID transid, char data);
  Transaction(salticidae::DataStream &data);

  CID getCid();
  TID getTid();
  unsigned char* getData();

  void serialize(salticidae::DataStream &data) const;
  void unserialize(salticidae::DataStream &data);
  std::string toString() const;
  std::string prettyPrint() const;

  bool operator<(const Transaction& s) const;
  bool operator==(const Transaction& s) const;

  unsigned int size();
};


#endif
