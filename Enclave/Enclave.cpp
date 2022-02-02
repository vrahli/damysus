#include "EnclaveShare.h"


hash_t lockh = newHash();     // hash of the last locked block
View   lockv = 0;             // lockh's view
hash_t preph = newHash();     // hash of the last prepared block
View   prepv = 0;             // preph's view
View   view  = 0;             // current view
Phase1 phase = PH1_NEWVIEW;   // current phase



// increments the (view,phase) pair
void increment() {
  if (phase == PH1_NEWVIEW) {
    phase = PH1_PREPARE;
  } else if (phase == PH1_PREPARE) {
    phase = PH1_PRECOMMIT;
  } else if (phase == PH1_PRECOMMIT) {
    phase = PH1_COMMIT;
  } else if (phase == PH1_COMMIT) {
    phase = PH1_NEWVIEW;
    view++;
  }
}



just_t sign(hash_t h1, hash_t h2, View v2) {
  rdata_t rdata;
  rdata.proph = h1; rdata.propv = view; rdata.justh = h2; rdata.justv = v2; rdata.phase = phase;
  sign_t sign = signString(rdata2string(rdata));
  signs_t signs; signs.size = 1; signs.signs[0] = sign;
  just_t j; j.set = 1; j.rdata = rdata; j.signs = signs;

  increment();

  return j;
}


sgx_status_t TEEsign(just_t *just) {
  sgx_status_t status = SGX_SUCCESS;
  hash_t hash = noHash();

  *just = sign(hash,preph,prepv);

  return status;
}

sgx_status_t TEEverify(just_t *just, unsigned int *res) {
  sgx_status_t status = SGX_SUCCESS;
  bool b = verifyJust(just);
  *res=b;
  return status;
}

sgx_status_t TEEprepare(hash_t *hash, just_t *just, just_t *res) {
  //ocall_print("TEEprepare...");
  sgx_status_t status = SGX_SUCCESS;
  rdata_t rd = just->rdata;
  hash_t  h2 = rd.justh;
  View    v2 = rd.justv;
  Phase1  ph = rd.phase;
  if (verifyJust(just)
      && view == rd.propv
      && ph == PH1_NEWVIEW
      && (eqHashes(h2,lockh) || v2 > lockv)) {
    *res = sign(*hash,h2,v2);
  } else { res->set=false; }
  return status;
}

sgx_status_t TEEstore(just_t *just, just_t *res) {
  //ocall_print("TEEstore...");
  sgx_status_t status = SGX_SUCCESS;
  rdata_t rd = just->rdata;
  hash_t  h  = rd.proph;
  View    v  = rd.propv;
  Phase1  ph = rd.phase;
  if (just->signs.size == getQsize()
      && verifyJust(just)
      && view == v
      && (ph == PH1_PREPARE || ph == PH1_PRECOMMIT)) {
    preph=h; prepv=v;
    if (PH1_PRECOMMIT) { lockh=h; lockv=v; }
    *res = sign(h,newHash(),0);
  } else { res->set=false; }
  return status;
}
