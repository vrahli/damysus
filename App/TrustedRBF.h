#ifndef TRUSTEDRBF_H
#define TRUSTEDRBF_H


#include "Hash.h"
#include "Just.h"
#include "Accum.h"
#include "../Enclave/user_types.h"


class TrustedRBF {

 private:
  Hash   preph;          // hash of the last prepared block
  View   prepv;          // preph's view
  View   view;           // current view
  Phase1 phase;          // current phase
  PID    id;             // unique identifier
  KEY    priv;           // private key
  unsigned int qsize;    // quorum size
  // TODO: create store for all seen counters
  // TODO: create method for updating counter
  // TODO: create onStart method
  // TODO: add stable predicate

  Just sign(Hash h1, Hash h2, View v2);
  void increment();

 public:
  TrustedRBF();
  TrustedRBF(unsigned int id, KEY priv, unsigned int q);

  Just TEEsign();
  Just TEEprepare(Stats &stats, Nodes nodes, Hash hash, Accum acc);
  Just TEEstore(Stats &stats, Nodes nodes, Just just);
  Accum TEEaccum(Stats &stats, Nodes nodes, Just justs[MAX_NUM_SIGNATURES]);
  Accum TEEaccumSp(Stats &stats, Nodes nodes, just_t just);
};


#endif