





#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "TrustedCh.h"


TrustedCh::TrustedCh() {
  this->lockh  = JBlock().hash(); // the genesis block
  this->lockv  = 0;
  this->preph  = JBlock().hash(); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->qsize  = 0;
}

TrustedCh::TrustedCh(PID id, KEY priv, unsigned int q) {
  this->lockh  = JBlock().hash(); // the genesis block
  this->lockv  = 0;
  this->preph  = JBlock().hash(); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->id     = id;
  this->priv   = priv;
  this->qsize  = q;
}


// increments the (view,phase) pair
void TrustedCh::increment() {
  if (this->phase == PH1_PREPARE) {
    this->phase = PH1_NEWVIEW;
  } else if (this->phase == PH1_NEWVIEW) {
    this->phase = PH1_PREPARE;
    this->view++;
  }
}


Just TrustedCh::sign(Hash h1, Hash h2, View v2) {
  RData rdata(h1,this->view,h2,v2,this->phase);
  Sign sign(this->priv,this->id,rdata.toString());
  Just just(rdata,sign);

  increment();

  return just;
}


Just TrustedCh::TEEsign() {
  return sign(Hash(false),this->preph,this->prepv);
}


bool verify(Stats &stats, PID id, Nodes nodes, Just just) {
  return just.getSigns().verify(stats,id,nodes,just.getRData().toString());
}


Just TrustedCh::TEEprepare(Stats &stats, Nodes nodes, JBlock block, JBlock block0, JBlock block1) {
  Just just = block.getJust();
  Just just0 = block0.getJust();
  bool vb = verify(stats,this->id,nodes,just);
  if (vb
      && this->view == just.getRData().getPropv()+1
      && just.wellFormed(this->qsize)
      && just0.wellFormed(this->qsize)) {

    if (just.getCHash() == block0.hash()) {
      this->preph = just.getCHash();
      this->prepv = just.getCView();

      if (just0.getCHash() == block1.hash()) {
        this->lockh = just0.getCHash();
        this->lockv = just0.getCView();
      }
    }

    // SafeNode check
    if (just.getCHash() == this->lockh || just.getCView() > this->lockv) {
      return sign(block.hash(),Hash(),View());
    } else {
      if (DEBUG) { std::cout << KMAG << "unsafe:"
                             << "hash="  << std::to_string(just.getCHash() == this->lockh) << "(" << just.getCHash().toString() << "," << this->lockh.toString() << ")"
                             << ";view=" << std::to_string(just.getCView() > this->lockv)
                             << KNRM << std::endl; }
    }
  } else {
    if (DEBUG) { std::cout << KMAG << "TEEprepare failed:"
                           << "id="        << std::to_string(this->id)
                           << ";verif="    << std::to_string(vb)
                           << ";eq-views=" << std::to_string(this->view == just.getRData().getPropv()+1) << "(" << this->view << "," << just.getRData().getPropv()+1 << ")"
                           << ";wf="       << std::to_string(just.wellFormed(this->qsize))
                           << ";wf0="      << std::to_string(just0.wellFormed(this->qsize))
                           << ";chain="    << std::to_string(just.getCHash() == block0.hash())
                           << ";chain0="   << std::to_string(just0.wellFormedInit() || just0.getCHash() == block1.hash())
                           << KNRM << std::endl; }
  }

  // otherwise we return the dummy justification
  return Just();
}
