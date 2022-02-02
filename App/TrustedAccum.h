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

  bool verifyCData(Nodes nodes, CData<Void,Cert> data, Sign sign);
  bool verifyCert(Nodes nodes, Cert c);

 public:
  TrustedAccum();
  TrustedAccum(PID id, KEY priv, unsigned int qsize);

  Accum TEEaccum(Nodes nodes, Vote<Void,Cert> votes[MAX_NUM_SIGNATURES]);
  Accum TEEaccumSp(Nodes nodes, uvote_t vote);
};


#endif
