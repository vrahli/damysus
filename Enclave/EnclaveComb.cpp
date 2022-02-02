#include <set>
#include "EnclaveShare.h"


hash_t COMBpreph = newHash(); // hash of the last prepared block
View   COMBprepv = 0;             // preph's view
View   COMBview  = 0;             // current view
Phase1 COMBphase = PH1_NEWVIEW;   // current phase



// increments the (view,phase) pair
void COMB_increment() {
  if (COMBphase == PH1_NEWVIEW) {
    COMBphase = PH1_PREPARE;
  } else if (COMBphase == PH1_PREPARE) {
    COMBphase = PH1_PRECOMMIT;
  } else if (COMBphase == PH1_PRECOMMIT) {
    COMBphase = PH1_NEWVIEW;
    COMBview++;
  }
}


just_t COMB_sign(hash_t h1, hash_t h2, View v2) {
  rdata_t rdata;
  rdata.proph = h1; rdata.propv = COMBview; rdata.justh = h2; rdata.justv = v2; rdata.phase = COMBphase;
  sign_t sign = signString(rdata2string(rdata));
  signs_t signs; signs.size = 1; signs.signs[0] = sign;
  just_t j; j.set = 1; j.rdata = rdata; j.signs = signs;

  COMB_increment();

  return j;
}


sgx_status_t COMB_TEEsign(just_t *just) {
  sgx_status_t status = SGX_SUCCESS;
  hash_t hash = noHash();

  *just = COMB_sign(hash,COMBpreph,COMBprepv);

  return status;
}

sgx_status_t COMB_TEEprepare(hash_t *hash, accum_t *acc, just_t *res) {
  //ocall_print("TEEprepare...");
  sgx_status_t status = SGX_SUCCESS;

  //if (DEBUG0) { ocall_print((nfo() + "COMB_TEEprepare hash:" + hash->toString()).c_str()); }

  if (verifyAccum(acc)
      && COMBview == acc->view
      && acc->size == getQsize()) {
    *res = COMB_sign(*hash,acc->hash,acc->prepv);
  } else { res->set = false; }
  return status;
}


sgx_status_t COMB_TEEstore(just_t *just, just_t *res) {
  //ocall_print("TEEstore...");
  sgx_status_t status = SGX_SUCCESS;
  rdata_t rd = just->rdata;
  hash_t  h  = rd.proph;
  View    v  = rd.propv;
  Phase1  ph = rd.phase;
  if (just->signs.size == getQsize()
      && verifyJust(just)
      && COMBview == v
      && ph == PH1_PREPARE) {
    COMBpreph=h; COMBprepv=v;
    *res = COMB_sign(h,newHash(),0);
  } else { res->set=false; }
  return status;
}


sgx_status_t COMB_TEEaccum(onejusts_t *js, accum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  View v = js->justs[0].rdata.propv;
  View highest = 0;
  hash_t hash = newHash();
  std::set<PID> signers;

  for (int i = 0; i < MAX_NUM_SIGNATURES && i < getQsize(); i++) {
    onejust_t just  = js->justs[i];
    rdata_t   data  = just.rdata;
    sign_t    sign  = just.sign;
    signs_t   signs; signs.size = 1; signs.signs[0] = sign;
    PID signer = sign.signer;
    if (data.phase == PH1_NEWVIEW
        && data.propv == v
        && signers.find(signer) == signers.end()
        && verifyText(signs,rdata2string(data))) {
      signers.insert(signer);
      if (data.justv >= highest) {
        highest = data.justv;
        hash = data.justh;
      }
    }
  }

  bool set = true;
  unsigned int size = signers.size();
  std::string text = std::to_string(set) + std::to_string(v) + std::to_string(highest) + hash2string(hash) + std::to_string(size);
  sign_t sign = signString(text);
  res->set = 1;
  res->view = v;
  res->prepv = highest;
  res->hash = hash;
  res->size = size;
  res->sign = sign;

  return status;
}


sgx_status_t COMB_TEEaccumSp(just_t *just, accum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  rdata_t rdata = just->rdata;
  signs_t signs = just->signs;
  hash_t  proph = rdata.proph;
  View    propv = rdata.propv;
  hash_t  justh = rdata.justh;
  View    justv = rdata.justv;
  Phase1  phase = rdata.phase;

  std::set<PID> signers;

  if (phase == PH1_NEWVIEW && verifyText(signs,rdata2string(rdata))) {
    for (int i = 0; i < MAX_NUM_SIGNATURES && i < getQsize() && i < signs.size; i++) {
      PID signer = signs.signs[i].signer;
      if (signers.find(signer) == signers.end()) { signers.insert(signer); }
    }
  }

  bool set = true;
  unsigned int size = signers.size();
  std::string text = std::to_string(set) + std::to_string(propv) + std::to_string(justv) + hash2string(justh) + std::to_string(size);
  sign_t sign = signString(text);
  res->set = 1;
  res->view = propv;
  res->prepv = justv;
  res->hash = justh;
  res->size = size;
  res->sign = sign;

  return status;
}
