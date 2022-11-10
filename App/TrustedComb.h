#ifndef TRUSTEDCOMB_H
#define TRUSTEDCOMB_H


#include "Hash.h"
#include "Just.h"
#include "Accum.h"
#include "../Enclave/user_types.h"


class TrustedComb {

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
  TrustedComb();
  TrustedComb(unsigned int id, KEY priv, unsigned int q);

  Just TEEsign();
  Just TEEprepare(Stats &stats, Nodes nodes, Hash hash, Accum acc);
  Just TEEstore(Stats &stats, Nodes nodes, Just just);
  Accum TEEaccum(Stats &stats, Nodes nodes, Just justs[MAX_NUM_SIGNATURES]);
  Accum TEEaccumSp(Stats &stats, Nodes nodes, just_t just);
};


#endif
