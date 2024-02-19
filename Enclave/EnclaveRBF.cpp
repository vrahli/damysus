#include <set>
#include "EnclaveShare.h"


hash_t RBFpreph = newHash(); // hash of the last prepared block
View   RBFprepv = 0;             // preph's view
View   RBFview  = 0;             // current view
Phase1 RBFphase = PH1_NEWVIEW;   // current phase
std::map<PID, sign_t> MCs; // Logged monotonic counters



// increments the (view,phase) pair
void RBF_increment() {
  if (RBFphase == PH1_NEWVIEW) {
    RBFphase = PH1_PREPARE;
  } else if (RBFphase == PH1_PREPARE) {
    RBFphase = PH1_PRECOMMIT;
  } else if (RBFphase == PH1_PRECOMMIT) {
    RBFphase = PH1_NEWVIEW;
    RBFview++;
  }
}

just_t RBF_sign(hash_t h1, hash_t h2, View v2) {
  rdata_t rdata;
  rdata.proph = h1; rdata.propv = RBFview; rdata.justh = h2; rdata.justv = v2; rdata.phase = RBFphase;
  sign_t sign = signString(rdata2string(rdata));
  signs_t signs; signs.size = 1; signs.signs[0] = sign;
  just_t j; j.set = 1; j.rdata = rdata; j.signs = signs;

  RBF_increment();

  return j;
}

//TODO: log the MC values, and the message in runtime memory
sgx_status_t RBF_TEEsign(just_t *just) {
  sgx_status_t status = SGX_SUCCESS;
  hash_t hash = noHash();

  *just = RBF_sign(hash,RBFpreph,RBFprepv);

  return status;
}

//TODO: log the MC values, and the message in runtime memory
sgx_status_t RBF_TEEprepare(hash_t *hash, accum_t *acc, just_t *res) {
  //ocall_print("TEEprepare...");
  sgx_status_t status = SGX_SUCCESS;

  //if (DEBUG0) { ocall_print((nfo() + "RBF_TEEprepare hash:" + hash->toString()).c_str()); }

  if (verifyAccum(acc)
      && RBFview == acc->view
      && acc->size == getQsize()) {
    *res = RBF_sign(*hash,acc->hash,acc->prepv);
  } else { res->set = false; }
  return status;
}

//TODO: log the MC values, and the message in runtime memory
sgx_status_t RBF_TEEstore(just_t *just, just_t *res) {
  //ocall_print("TEEstore...");
  sgx_status_t status = SGX_SUCCESS;
  rdata_t rd = just->rdata;
  hash_t  h  = rd.proph;
  View    v  = rd.propv;
  Phase1  ph = rd.phase;
  if (just->signs.size == getQsize()
      && verifyJust(just)
      && RBFview == v
      && ph == PH1_PREPARE) {
    for (int i = 0; i < just->signs.size; i++) {
      sign_t sign = just->signs.signs[i];
      PID signer = sign.signer;
      MCs.at(signer) = sign;
    }
    RBFpreph=h; RBFprepv=v;
    *res = RBF_sign(h,newHash(),0);
  } else { res->set=false; }
  return status;
}

//TODO: log the MC values, and the message in runtime memory
sgx_status_t RBF_TEEaccum(onejusts_t *js, accum_t *res) {
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

//TODO: log the MC values, and the message in runtime memory
sgx_status_t RBF_TEEaccumSp(just_t *just, accum_t *res) {
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

//Allow for a recovery of the SGX enclave, iff a quorum of non-zero values is supplied
sgx_status_t RBF_TEErecovery(just_t *just, just_t *res) {
  //set values to maximum value, if a quorum is reached.
  
  //ocall_print("TEEstore...");
  sgx_status_t status = SGX_SUCCESS;
  rdata_t rd = just->rdata;
  hash_t  h  = rd.proph;
  View    v  = rd.propv;
  Phase1  ph = rd.phase;
  if (just->signs.size == getQsize() //TODO: change condition
      && verifyJust(just)
      && RBFview == v
      && ph == PH1_PREPARE) {
      RBFview = 0; //TODO: change to max found value
      RBFphase = 0; //TODO: change to max found value
    RBFpreph=h; RBFprepv=v;
    *res = RBF_sign(h,newHash(),0);
  } else { res->set=false; }
  return status;
}

//supply a requested monotonic counter with a proof of a message
sgx_status_t RBF_TEEsupplyMC(){

}
