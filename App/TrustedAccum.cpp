#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <cstring>


#include "TrustedAccum.h"


TrustedAccum::TrustedAccum() {}


TrustedAccum::TrustedAccum(PID id, KEY priv, unsigned int qsize) {
  this->id    = id;
  this->priv  = priv;
  this->qsize = qsize;
}


bool TrustedAccum::verifyCData(Stats &stats, Nodes nodes, CData<Void,Cert> data, Sign sign) {
  Signs signs = Signs(sign);
  return signs.verify(stats,this->id,nodes,data.toString());
}


// When verifying certificates, we have to verify MsgPrepareAcc's as certificates are generated from those
bool TrustedAccum::verifyCert(Stats &stats, Nodes nodes, Cert c) {
  CData<Hash,Void> data(PH1_PREPARE,c.getView(),c.getHash(),Void());
  return c.getSigns().verify(stats,this->id,nodes,data.toString());
}


Accum TrustedAccum::TEEaccum(Stats &stats, Nodes nodes, Vote<Void,Cert> votes[MAX_NUM_SIGNATURES]) {
  View v = votes[0].getCData().getView();
  View highest = 0;
  Hash hash = Hash();
  std::set<PID> signers;

  for (int i = 0; i < MAX_NUM_SIGNATURES && i < this->qsize; i++) {
    Vote<Void,Cert> vote = votes[i];
    CData<Void,Cert> data = vote.getCData();
    Sign sign  = vote.getSign();
    PID signer = sign.getSigner();
    Cert cert  = data.getCert();
    bool vd = verifyCData(stats,nodes,data,sign);
    bool vc = verifyCert(stats,nodes,cert);
    if (data.getPhase() == PH1_NEWVIEW
        && data.getView() == v
        && signers.find(signer) == signers.end()
        && vd
        && vc) {
      if (DEBUG1) std::cout << KMAG << "[" << this->id << "]" << "inserting signer" << KNRM << std::endl;
      signers.insert(signer);
      if (cert.getView() >= highest) {
        highest = cert.getView();
        hash = cert.getHash();
      }
    } else {
      if (DEBUG1) std::cout << KMAG << "[" << this->id << "]" << "vote:" << vote.prettyPrint() << KNRM << std::endl;
      if (DEBUG1) std::cout << KMAG << "[" << this->id << "]" << "not inserting signer (" << signer << ") because:"
                            << "check-phase=" << std::to_string(data.getPhase() == PH1_NEWVIEW) << "(" << data.getPhase() << "," << PH1_NEWVIEW << ")"
                            << ";check-view=" << std::to_string(data.getView() == v)
                            << ";check-notin=" << std::to_string(signers.find(signer) == signers.end())
                            << ";verif-data=" << std::to_string(vd)
                            << ";verif-cert=" << std::to_string(vc)
                            << KNRM << std::endl;
    }
  }

  bool set = true;
  unsigned int size = signers.size();
  std::string text = std::to_string(set) + std::to_string(v) + std::to_string(highest) + hash.toString() + std::to_string(size);
  Sign sign(this->priv,this->id,text);
  return Accum(v,highest,hash,size,sign);
}


Accum TrustedAccum::TEEaccumSp(Stats &stats, Nodes nodes, uvote_t vote) {
  cdata_t cdata = vote.cdata;
  signs_t ss = vote.signs;
  // extracted from cdata:
  Phase1 p = cdata.phase;
  View v = cdata.view;
  cert_t c = cdata.cert;
  // extracted from c:
  View highest = c.view;
  hash_t hash = c.hash;

  std::set<PID> signers;

  Hash h = Hash(hash.set,hash.hash);
  Sign a[MAX_NUM_SIGNATURES];
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) {
    a[i]=Sign(c.signs.signs[i].set,c.signs.signs[i].signer,c.signs.signs[i].sign);
  }
  Signs signs(c.signs.size,a);
  Cert cert = Cert(highest,h,signs);
  bool vc = verifyCert(stats,nodes,cert);

  if (p == PH1_NEWVIEW && vc) {
    std::string data = std::to_string(p) + std::to_string(v) + cert.toString();

    for (int i = 0; i < MAX_NUM_SIGNATURES && i < this->qsize && i < ss.size; i++) {
      PID signer = ss.signs[i].signer;
      Signs sign = Sign(ss.signs[i].set,signer,ss.signs[i].sign);
      bool vd = Signs(sign).verify(stats,this->id,nodes,data);
      if (vd && signers.find(signer) == signers.end()) { signers.insert(signer); }
    }
  }

  bool set = true;
  unsigned int size = signers.size();
  std::string text = std::to_string(set) + std::to_string(v) + std::to_string(highest) + h.toString() + std::to_string(size);
  Sign sign(this->priv,this->id,text);
  return Accum(v,highest,h,size,sign);
}
