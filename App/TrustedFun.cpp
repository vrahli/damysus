#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "TrustedFun.h"


TrustedFun::TrustedFun() {
  this->lockh  = Hash(true); // the genesis block
  this->lockv  = 0;
  this->preph  = Hash(true); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->qsize  = 0;
}

TrustedFun::TrustedFun(PID id, KEY priv, unsigned int q) {
  this->lockh  = Hash(true); // the genesis block
  this->lockv  = 0;
  this->preph  = Hash(true); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->id     = id;
  this->priv   = priv;
  this->qsize  = q;
}


// increments the (view,phase) pair
void TrustedFun::increment() {
  if (this->phase == PH1_NEWVIEW) {
    this->phase = PH1_PREPARE;
  } else if (this->phase == PH1_PREPARE) {
      this->phase = PH1_PRECOMMIT;
  } else if (this->phase == PH1_PRECOMMIT) {
    this->phase = PH1_COMMIT;
  } else if (this->phase == PH1_COMMIT) {
    this->phase = PH1_NEWVIEW;
    this->view++;
  }
}


Just TrustedFun::sign(Hash h1, Hash h2, View v2) {
  RData rdata(h1,this->view,h2,v2,this->phase);
  Sign sign(this->priv,this->id,rdata.toString());
  Just just(rdata,sign);

  increment();

  return just;
}


Just TrustedFun::TEEsign(Stats &stats) {
  auto start = std::chrono::steady_clock::now();
  Just j = sign(Hash(false),this->preph,this->prepv);
  auto end = std::chrono::steady_clock::now();
  double time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  stats.addCryptoSignTime(time);
  return j;
}

bool TrustedFun::TEEverify(Stats &stats, Nodes nodes, Just just) {
  auto start = std::chrono::steady_clock::now();
  bool b = just.getSigns().verify(stats,this->id,nodes,just.getRData().toString());
  auto end = std::chrono::steady_clock::now();
  double time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  stats.addCryptoVerifTime(time);
  return b;
}

Just TrustedFun::TEEprepare(Stats &stats, Nodes nodes, Hash hash, Just just) {
  RData  rd = just.getRData();
  Hash   h2 = rd.getJusth();
  View   v2 = rd.getJustv();
  Phase1 ph = rd.getPhase();
  if (TEEverify(stats,nodes,just)
      && this->view == rd.getPropv()
      && ph == PH1_NEWVIEW
      && (h2 == this->lockh || v2 > this->lockv)) {
    return sign(hash,h2,v2);
  } else {
    if (DEBUG) std::cout << KMAG << "TEEprepare failed:"
                         << "TEEverif="   << std::to_string(TEEverify(stats,nodes,just))
                         << ";eq-views="  << std::to_string(this->view == rd.getPropv())
                         << ";eq-phases=" << std::to_string(ph == PH1_NEWVIEW)
                         << ";safety="     << std::to_string((h2 == this->lockh || v2 > this->lockv))
                         << KNRM << std::endl;
  }
  return Just();
}

Just TrustedFun::TEEstore(Stats &stats, Nodes nodes, Just just) {
  Hash   h  = just.getRData().getProph();
  View   v  = just.getRData().getPropv();
  Phase1 ph = just.getRData().getPhase();
  if (just.getSigns().getSize() == this->qsize
      && TEEverify(stats,nodes,just)
      && this->view == v
      && (ph == PH1_PREPARE || ph == PH1_PRECOMMIT)) {
    this->preph=h; this->prepv=v;
    if (PH1_PRECOMMIT) { this->lockh=h; this->lockv=v; }
    return sign(h,Hash(),View());
  }
  return Just();
}
