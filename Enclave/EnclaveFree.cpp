#include "Enclave_t.h"

#include <openssl/pem.h>
#include <openssl/err.h>

#include <string>
//#include <map>
#include <set>

//#include "EnclaveShare.h"
#include "../App/types.h"


View FREEprepv = 0;      // preph's view
View FREEview  = 0;      // current view
bool FREEphase = PH2A;   // current phase
PID  FREEid    = 0;
unsigned int FREEqsize;     // quorum size -- from EnclaveShare.cpp

const char FREEsecret[] = {
  "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgnI0T6AoPs+ufh54e\n"
  "3tr6ywY7KkMBZhBs69NvMpvtXeehRANCAAS+G04ABpuwCvaS0v5fi9vuNOEitPon\n"
  "4nIDK/IJOsGXv85Jw5wayZI19lSB6ox05rLB+CxmEXrDyiOhX8Sz7c0L\n"
};


sgx_status_t FREE_initialize_variables(PID *me, unsigned int *q) {
  sgx_status_t status = SGX_SUCCESS;

  FREEid = *me;
  ocall_print(("ENCLAVE:set up id-" + std::to_string(FREEid)).c_str());

  FREEqsize = *q;
  ocall_print(("ENCLAVE:set up the quorum size-" + std::to_string(FREEqsize)).c_str());

  return status;
}


// increments the (view,phase) pair
void FREE_increment() {
  if (FREEphase == PH2A) {
    FREEphase = PH2B;
  } else {
    FREEphase = PH2A;
    FREEview++;
  }
}


auth_t free_authenticate(unsigned int i, std::string text) {
  //ocall_print(("[" + std::to_string(FREEid) + "]ENCLAVE:authenticating:" + std::to_string(i)).c_str());

  if (DEBUGOT) ocall_setCtime();
  auth_t auth;
  std::string s = std::to_string(FREEid) + FREEsecret + text;
  if (!SHA256 ((const unsigned char *)s.c_str(), s.size(), auth.hash.hash)) { }
  auth.hash.set=true;
  auth.id = FREEid;
  if (DEBUGOT) ocall_recCStime();

  return auth;
}


sgx_status_t FREE_TEEauth(payload_t *p, auth_t *res) {
  sgx_status_t status = SGX_SUCCESS;
  std::string text = "";
  for (int i = 0; i < p->size; i++) { text += ((char *)p->data)[i]; }
  //ocall_print(("ENCLAVE:authenticating:" + text).c_str());
  *res = free_authenticate(0,text);
  return status;
}



bool free_sameHashes(unsigned char *h1, unsigned char *h2) {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    if (h1[i] != h2[i]) { return false; }
  }
  return true;
}


bool free_verify(std::string text, auth_t a) {
  if (DEBUGOT) ocall_setCtime();
  unsigned char hash[SHA256_DIGEST_LENGTH];
  std::string s = std::to_string(a.id) + FREEsecret + text;
  if (!SHA256 ((const unsigned char *)s.c_str(), s.size(), hash)) { }
  bool b = free_sameHashes(a.hash.hash,hash);
  if (DEBUGOT) ocall_recCVtime();

  return b;
}


bool free_verifyAuths(std::string text, auths_t auths) {
  for (int i = 0; i < auths.size; i++) {
    if (!free_verify(text,auths.auths[i])) { return false; }
  }
  return true;
}


sgx_status_t FREE_TEEverify(payload_t *p, auths_t *a, bool *res) {
  sgx_status_t status = SGX_SUCCESS;
  std::string text;
  for (int i = 0; i < p->size; i++) { text += (p->data)[i]; }
  *res = free_verifyAuths(text, *a);
  return status;
}


sgx_status_t FREE_TEEverify2(payload_t *p1, auths_t *a1, payload_t *p2, auths_t *a2, bool *res) {
  sgx_status_t status = SGX_SUCCESS;
  std::string text1;
  for (int i = 0; i < p1->size; i++) { text1 += (p1->data)[i]; }
  bool b1 = free_verifyAuths(text1, *a1);
  std::string text2;
  for (int i = 0; i < p2->size; i++) { text2 += (p2->data)[i]; }
  bool b2 = free_verifyAuths(text2, *a2);
  *res = b1 && b2;
  return status;
}


// From EnclaveShare.cpp
std::string free_hash2str(hash_t hash) {
  std::string text;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { text += hash.hash[i]; }
  text += std::to_string(hash.set);
  return text;
}


std::string free_H2S(hash_t hash) {
  std::string text;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { text += std::to_string((int)hash.hash[i]); }
  return text;
}


sgx_status_t FREE_TEEprepare(hash_t *hash, hjust_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  if (FREEphase == PH2B) {
    std::string sh = free_hash2str(*hash);
    std::string text = sh + std::to_string(FREEview);
    //ocall_print(("ENCLAVE:preparing-hash(" + std::to_string(sh.size()) + "):-" + sh + "-").c_str());
    //ocall_print(("ENCLAVE:preparing-hash:-" + free_H2S(*hash) + "-").c_str());
    //ocall_print(("ENCLAVE:preparing-view:" + std::to_string(FREEview)).c_str());
    res->auth = free_authenticate(1,text);
    res->set=true;
    res->hash=*hash;
    res->view=FREEview;
    FREE_increment();
  } else { res->set = false; }
  return status;
}


bool free_verifyOrInit(pjust_t *just) {
  // We don't verify the initial justification, whch is a dummy one
  if (FREEview == 0 && FREEphase == PH2A && FREEprepv == 0 && just->view == 0 && just->hash.set == false) {
    return true;
  }
  std::string text1 = free_hash2str(just->hash) + std::to_string(just->view);
  std::string text2 = std::to_string(PH2A) + std::to_string(just->view);
  bool b1 = free_verify(text1,just->auth);
  bool b2 = free_verifyAuths(text2,just->auths);
  //ocall_print(("ENCLAVE:free_verifyOrInit:" + std::to_string(b1) + ":" + std::to_string(b2)).c_str());
  //ocall_print(("ENCLAVE:free_verifyOrInit:" + text2).c_str());
  return b1 && b2;
}


sgx_status_t FREE_TEEstore(pjust_t *just, fvjust_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  if (free_verifyOrInit(just) && just->view >= FREEprepv) {
    //ocall_print(("ENCLAVE:storing" + std::to_string(FREEid)).c_str());
    if (FREEphase == PH2B) { FREEview++; FREEphase = PH2A; }
    FREEprepv = just->view;
    res->auth1 = free_authenticate(2,std::to_string(FREEphase) + std::to_string(FREEview));
    res->auth2 = free_authenticate(3,std::to_string(just->view) + free_hash2str(just->hash) + std::to_string(FREEview));
    res->data.justh=just->hash;
    res->data.justv=just->view;
    res->data.view=FREEview;
    res->set=true;
    FREE_increment();
    //ocall_print(("ENCLAVE:stored with auth (" + std::to_string(res->auth2.id) + ")" + std::to_string(FREEid)).c_str());
    //ocall_print(("ENCLAVE:stored" + std::to_string(FREEid)).c_str());
  } else { res->set=false; }

  return status;
}


// From EnclaveShare.cpp
hash_t free_mkNewHash() {
  hash_t hash;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { hash.hash[i] = '0'; }
  hash.set=true;
  return hash;
}


sgx_status_t FREE_TEEaccum(fjust_t *j, fjusts_t *js, hash_t *prp, haccum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  View v = j->data.view;
  View highest = j->data.justv;
  hash_t hash = j->data.justh;
  std::set<PID> signers;
  signers.insert(j->auth.id);

  for (int i = 0; i < MAX_NUM_SIGNATURES-1 && i < FREEqsize-1; i++) {
    fjust_t just   = js->justs[i];
    fdata_t data   = just.data;
    auth_t  auth   = just.auth;
    PID     signer = auth.id;
    std::string text = std::to_string(data.justv) + free_hash2str(data.justh) + std::to_string(data.view);
    if (data.view == v
        && signers.find(signer) == signers.end()
        && free_verify(text,auth)
        && highest >= data.justv) {
      signers.insert(signer);
    }
  }

  unsigned int size = signers.size();
  //ocall_print(("ENCLAVE:TEEaccum" + std::to_string(size)).c_str());
  std::string text = std::to_string(true) + std::to_string(v) + free_hash2str(hash) + std::to_string(size);
  res->auth = free_authenticate(4,text);
  res->set = true;
  res->view = v;
  res->hash = hash;
  res->size = size;

  // prepare part
  if (FREEphase == PH2B && v == FREEview) {
    std::string sh = free_hash2str(*prp);
    std::string textp = sh + std::to_string(FREEview);
    res->authp = free_authenticate(5,textp);
  } else { res->set = false; }

  return status;
}


sgx_status_t FREE_TEEaccumSp(ofjust_t *just, hash_t *prp, haccum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  fdata_t data  = just->data;
  auths_t auths = just->auths;
  View    view  = data.view;
  hash_t  justh = data.justh;
  View    justv = data.justv;

  std::set<PID> signers;
  std::string text = std::to_string(justv) + free_hash2str(justh) + std::to_string(view);

  for (int i = 0; i < MAX_NUM_SIGNATURES && i < FREEqsize && i < auths.size; i++) {
    auth_t auth = auths.auths[i];
    PID signer = auth.id;
    if (signers.find(signer) == signers.end() && free_verify(text,auth)) { signers.insert(signer); }
  }

  unsigned int size = signers.size();
  //ocall_print(("ENCLAVE:FREE_TEEaccumSp:" + std::to_string(auths.size) + ":" + std::to_string(size)).c_str());
  std::string atext = std::to_string(true) + std::to_string(view) + free_hash2str(justh) + std::to_string(size);
  res->auth = free_authenticate(6,atext);
  res->set = true;
  res->view = view;
  res->hash = justh;
  res->size = size;

  // prepare part
  if (FREEphase == PH2B && view == FREEview) {
    std::string sh = free_hash2str(*prp);
    std::string textp = sh + std::to_string(FREEview);
    res->authp = free_authenticate(7,textp);
  } else { res->set = false; }

  return status;
}
