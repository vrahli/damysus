#include <set>
#include "EnclaveShare.h"


hash_t RBFpreph = newHash(); // hash of the last prepared block
View   RBFprepv = 0;             // preph's view
View   RBFview  = 0;             // current view
Phase1 RBFphase = PH1_NEWVIEW;   // current phase
float nonce = 0;                 // nonce to uniquely identify TEEs with the same keypair



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


sgx_status_t RBF_TEEsign(just_t *just) {
  sgx_status_t status = SGX_SUCCESS;
  hash_t hash = noHash();

  *just = RBF_sign(hash,RBFpreph,RBFprepv);
  
  return status;
}

//Prepare only updates the view of the leader, so should only update map for acc->sign->
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

sgx_status_t RBF_TEEstore(just_t *just, just_t *res) {
  // ocall_print("TEEstore...");
  sgx_status_t status = SGX_SUCCESS;
  rdata_t rd = just->rdata;
  hash_t  h  = rd.proph;
  View    v  = rd.propv;
  Phase1  ph = rd.phase;
  if (just->signs.size == getQsize()
      && verifyJust(just)
      && RBFview == v
      && ph == PH1_PREPARE) {
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

// Every node functions

//Allow for a recovery of the SGX enclave
sgx_status_t RBF_TEErecovery(accum_t *acc, just_t *res) {
  //set values to maximum value, if a quorum is reached.
  //ocall_print("TEEstore...");
  sgx_status_t status = SGX_SUCCESS;
  //send message recovery(nonce, sign) to all leaders view, ..., view+f+u+1 of suspected view

  return status;
}

//Create Wish function to craft message for all leader within an epoch
sgx_status_t RBF_TEEwish() {
  sgx_status_t status = SGX_SUCCESS;
  //send message wish(view[self], sign) to all leaders view, ..., view+f+u+1

  return status;
}

//Simulate receiving messages (similar to upon functions)

sgx_status_t RBF_TEEreceiveTC() {
  sgx_status_t status = SGX_SUCCESS;
  //send vote to a leader that sends a valid proposed TC

  return status;
}

sgx_status_t RBF_TEEreceiveQC() {
  sgx_status_t status = SGX_SUCCESS;
  //send vote to a leader that sends a valid proposed QC

  return status;
}


// LEADER FUNCTIONS

//Collect wish and recover messages and create a TC
sgx_status_t RBF_TEEleaderWish() {
  sgx_status_t status = SGX_SUCCESS;
  //receive bunch of wish and recover messages, combine into TC and send to all TEEs

  return status;
}

//Collect TC votes and create a quorum for next epoch
sgx_status_t RBF_TEEleaderCreateQuorum() {
  sgx_status_t status = SGX_SUCCESS;
  //receive TC votes and send QC for next epoch to all TEEs

  return status;
}


//Rollback functionality 

//create method to reset view and phase to earlier version or 0, 
//simulating a rollback
sgx_status_t RBF_TEEattemptrollback(View *v) {
  sgx_status_t status = SGX_SUCCESS;
  // reset values and create new nonce to simulate new TEE
  return status;
}

