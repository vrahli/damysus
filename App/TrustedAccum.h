#ifndef TACCUM_H
#define TACCUM_H


#include "Vote.h"
#include "CData.h"
#include "Accum.h"
#include "Cert.h"
#include "Void.h"
#include "../Enclave/user_types.h"


class TrustedAccum {

 private:
  KEY priv;
  PID id;
  unsigned int qsize;

  bool verifyCData(Stats &stats, Nodes nodes, CData<Void,Cert> data, Sign sign);
  bool verifyCert(Stats &stats, Nodes nodes, Cert c);

 public:
  TrustedAccum();
  TrustedAccum(PID id, KEY priv, unsigned int qsize);

  Accum TEEaccum(Stats &stats, Nodes nodes, Vote<Void,Cert> votes[MAX_NUM_SIGNATURES]);
  Accum TEEaccumSp(Stats &stats, Nodes nodes, uvote_t vote);
};


#endif
