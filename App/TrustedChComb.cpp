#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>

#include "TrustedChComb.h"


TrustedChComb::TrustedChComb() {
  this->preph  = CBlock().hash(); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->qsize  = 0;
}


TrustedChComb::TrustedChComb(PID id, KEY priv, unsigned int q) {
  this->preph  = CBlock().hash(); // the genesis block
  this->prepv  = 0;
  this->view   = 0;
  this->phase  = PH1_NEWVIEW;
  this->id     = id;
  this->priv   = priv;
  this->qsize  = q;
}


// increments the (view,phase) pair
void TrustedChComb::increment() {
  if (this->phase == PH1_PREPARE) {
    this->phase = PH1_NEWVIEW;
  } else if (this->phase == PH1_NEWVIEW) {
    this->phase = PH1_PREPARE;
    this->view++;
  }
}


Just TrustedChComb::sign(Hash h1, Hash h2, View v2) {
  RData rdata(h1,this->view,h2,v2,this->phase);
  Sign sign(this->priv,this->id,rdata.toString());
  Just just(rdata,sign);

  increment();

  return just;
}


Just TrustedChComb::TEEsign() {
  return sign(Hash(false),this->preph,this->prepv);
}


bool verify(Stats &stats, PID id, Nodes nodes, CA cert) {
  if (DEBUG) { std::cout << KMAG << "verifying:" << cert.prettyPrint() << KNRM << std::endl; }
  if (cert.tag == CERT) {
    // We have here to verify the corresponding justification that we generated the certificate from in 'checkNewJustChComb'
    RData rdata(cert.cert.getHash(),cert.cert.getView(),Hash(),View(),PH1_PREPARE);
    Just just(rdata,cert.cert.getSigns());
    return just.getSigns().verify(stats,id,nodes,just.getRData().toString());
  }
  // else
  return Signs(cert.accum.getSign()).verify(stats,id,nodes,cert.accum.data2string());
}


Just TrustedChComb::TEEprepare(Stats &stats, Nodes nodes, CBlock block, Hash hash) {
  CA cert = block.getCert();
  bool vb = verify(stats,this->id,nodes,cert);
  if (vb
      && this->view == cert.getCView()+1) {

    if (cert.getHash() == hash) {
      this->preph = cert.getHash();
      this->prepv = cert.getView();
    }

    return sign(block.hash(),Hash(),View());
  } else {
    if (DEBUG) { std::cout << KMAG << "TEEprepare failed:"
                           << "id="        << this->id
                           << ";verif="    << vb
                           << ";eq-views=" << (this->view == cert.getCView()+1) << "(" << this->view << "," << cert.getCView()+1 << ")"
                           << ";chain="    << (cert.getHash() == hash)
                           << KNRM << std::endl; }
  }

  // otherwise we return the dummy justification
  return Just();
}


Accum TrustedChComb::TEEaccum(Stats &stats, Nodes nodes, Just justs[MAX_NUM_SIGNATURES]) {
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


Accum TrustedChComb::TEEaccumSp(Stats &stats, Nodes nodes, just_t just) {
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
