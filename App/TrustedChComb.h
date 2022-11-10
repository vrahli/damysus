#ifndef TRUSTEDCHCOMB_H
#define TRUSTEDCHCOMB_H


#include "Hash.h"
#include "Just.h"
#include "CBlock.h"
#include "../Enclave/user_types.h"


class TrustedChComb {

 private:
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
  TrustedChComb();
  TrustedChComb(unsigned int id, KEY priv, unsigned int q);

  Just TEEsign();
  Just TEEprepare(Stats &stats, Nodes nodes, CBlock block, Hash hash);
  Accum TEEaccum(Stats &stats, Nodes nodes, Just justs[MAX_NUM_SIGNATURES]);
  Accum TEEaccumSp(Stats &stats, Nodes nodes, just_t just);
};

#endif
