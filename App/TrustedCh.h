#ifndef TRUSTEDCH_H
#define TRUSTEDCH_H


#include "Hash.h"
#include "JBlock.h"


class TrustedCh {

 private:
  Hash   lockh;          // hash of the last locked block
  View   lockv;          // lockh's view
  Hash   preph;          // hash of the last prepared block
  View   prepv;          // preph's view
  View   view;           // current view
  Phase1 phase;          // current phase
  PID    id;             // unique identifier
  KEY    priv;           // private key
  unsigned int qsize;    // quorum size

  Just sign(Hash h1, Hash h2, View v2);
  void increment();

 public:
  TrustedCh();
  TrustedCh(unsigned int id, KEY priv, unsigned int q);

  Just TEEsign();
  Just TEEprepare(Stats &stats, Nodes nodes, JBlock block, JBlock block0, JBlock block1);
};


#endif
