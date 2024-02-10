#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "TrustedComb.h"


TrustedComb::TrustedComb() {
  this->preph  = Hash(true); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->qsize  = 0;
}

TrustedComb::TrustedComb(PID id, KEY priv, unsigned int q) {
  this->preph  = Hash(true); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->id     = id;
  this->priv   = priv;
  this->qsize  = q;
}


// increments the (view,phase) pair
void TrustedComb::increment() {
  if (this->phase == PH1_NEWVIEW) {
    this->phase = PH1_PREPARE;
  } else if (this->phase == PH1_PREPARE) {
      this->phase = PH1_PRECOMMIT;
  } else if (this->phase == PH1_PRECOMMIT) {
    this->phase = PH1_NEWVIEW;
    this->view++;
  }
}


Just TrustedComb::sign(Hash h1, Hash h2, View v2) {
  RData rdata(h1,this->view,h2,v2,this->phase);
  Sign sign(this->priv,this->id,rdata.toString());
  Just just(rdata,sign);

  increment();

  return just;
}


Just TrustedComb::TEEsign() {
  return sign(Hash(false),this->preph,this->prepv);
}


Just TrustedComb::TEEprepare(Stats &stats, Nodes nodes, Hash hash, Accum acc) {
  Signs signs(acc.getSign());
  if (signs.verify(stats,this->id,nodes,acc.data2string())
      && this->view == acc.getView()
      && acc.getSize() == this->qsize) {
    return sign(hash,acc.getPreph(),acc.getPrepv());
  } else {
    if (DEBUG1) std::cout << KMAG << "[" << this->id << "]" << "TEEprepare failed because:"
                          << "verif=" << (signs.verify(stats,this->id,nodes,acc.data2string()))
                          << ";view=" << (this->view == acc.getView())
                          << ";acc="  << (acc.getSize() == this->qsize) << "(" << acc.getSize() << "," << this->qsize << ")"
                          << KNRM << std::endl;
  }
  return Just();
}


Just TrustedComb::TEEstore(Stats &stats, Nodes nodes, Just just) {
  RData  data  = just.getRData();
  Signs  signs = just.getSigns();
  Hash   h     = data.getProph();
  View   v     = data.getPropv();
  Phase1 ph    = data.getPhase();
  if (signs.getSize() == this->qsize
      && signs.verify(stats,this->id,nodes,data.toString())
      && this->view == v
      && ph == PH1_PREPARE) {
    this->preph=h; this->prepv=v;
    return sign(h,Hash(),View());
  } else {
    if (DEBUG1) std::cout << KMAG << "[" << this->id << "]" << "TEEstore failed because:"
                          << "size="   << (signs.getSize() == this->qsize)
                          << ";verif=" << (signs.verify(stats,this->id,nodes,data.toString()))
                          << ";vierw=" << (this->view == v)
                          << ";phase=" << (ph == PH1_PREPARE)
                          << KNRM << std::endl;
  }
  return Just();
}


Accum TrustedComb::TEEaccum(Stats &stats, Nodes nodes, Just justs[MAX_NUM_SIGNATURES]) {
  View v = justs[0].getRData().getPropv();
  View highest = 0;
  Hash hash = Hash();
  std::set<PID> signers;

  for (int i = 0; i < MAX_NUM_SIGNATURES && i < this->qsize; i++) {
    Just  just  = justs[i];
    RData data  = just.getRData();
    Signs signs = just.getSigns();
    if (signs.getSize() == 1) {
      Sign sign  = signs.get(0);
      PID signer = sign.getSigner();
      if (data.getPhase() == PH1_NEWVIEW
          && data.getPropv() == v
          && signers.find(signer) == signers.end()
          && signs.verify(stats,this->id,nodes,data.toString())) {
        if (DEBUG1) std::cout << KMAG << "[" << this->id << "]" << "inserting signer" << KNRM << std::endl;
        signers.insert(signer);
        if (data.getJustv() >= highest) {
          highest = data.getJustv();
          hash = data.getJusth();
        }
      }
    }
  }

  bool set = true;
  unsigned int size = signers.size();
  std::string text = std::to_string(set) + std::to_string(v) + std::to_string(highest) + hash.toString() + std::to_string(size);
  Sign sign(this->priv,this->id,text);
  return Accum(v,highest,hash,size,sign);
}


Accum TrustedComb::TEEaccumSp(Stats &stats, Nodes nodes, just_t just) {
  std::set<PID> signers;

  rdata_t rdata = just.rdata;
  signs_t signs = just.signs;
  Hash proph = Hash(rdata.proph.set,rdata.proph.hash);
  View propv = rdata.propv;
  Hash justh = Hash(rdata.justh.set,rdata.justh.hash);
  View justv = rdata.justv;
  Phase1 phase = rdata.phase;
  std::string data = proph.toString() + std::to_string(propv) + justh.toString() + std::to_string(justv) + std::to_string(phase);

  if (phase == PH1_NEWVIEW) {
    for (int i = 0; i < MAX_NUM_SIGNATURES && i < this->qsize && i < signs.size; i++) {
      PID signer = signs.signs[i].signer;
      Signs sign = Sign(signs.signs[i].set,signer,signs.signs[i].sign);
      bool vd = Signs(sign).verify(stats,this->id,nodes,data);
      if (signers.find(signer) == signers.end() && vd) { signers.insert(signer); }
    }
  }

  bool set = true;
  unsigned int size = signers.size();
  std::string text = std::to_string(set) + std::to_string(propv) + std::to_string(justv) + justh.toString() + std::to_string(size);
  Sign sign(this->priv,this->id,text);
  return Accum(propv,justv,justh,size,sign);
}
