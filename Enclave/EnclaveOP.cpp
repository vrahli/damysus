#include "Enclave_t.h"

#include <openssl/pem.h>
#include <openssl/err.h>

#include <string>
#include <map>
#include <set>

//#include "EnclaveShare.h"
#include "../App/types.h"



View OPprepv = 0;      // preph's view
View OPview  = 0;      // current view
OPphase OPph = OPpa;   // current phase
PID  OPid    = 0;
unsigned int OPqsize;     // quorum size -- from EnclaveShare.cpp

const char OPsecret[] = {
  "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgnI0T6AoPs+ufh54e\n"
  "3tr6ywY7KkMBZhBs69NvMpvtXeehRANCAAS+G04ABpuwCvaS0v5fi9vuNOEitPon\n"
  "4nIDK/IJOsGXv85Jw5wayZI19lSB6ox05rLB+CxmEXrDyiOhX8Sz7c0L\n"
};


sgx_status_t OP_initialize_variables(PID *me, unsigned int *q) {
  sgx_status_t status = SGX_SUCCESS;

  OPid = *me;
  ocall_print(("ENCLAVE:set up id-" + std::to_string(OPid)).c_str());

  OPqsize = *q;
  ocall_print(("ENCLAVE:set up the quorum size-" + std::to_string(OPqsize)).c_str());

  return status;
}


auth_t op_authenticate(std::string text) {
  auth_t auth;
  std::string s = std::to_string(OPid) + OPsecret + text;

  if (!SHA256 ((const unsigned char *)s.c_str(), s.size(), auth.hash.hash)) { }
  auth.hash.set=true;
  auth.id = OPid;

  return auth;
}


sgx_status_t OP_TEEauth(payload_t *p, auth_t *res) {
  sgx_status_t status = SGX_SUCCESS;
  std::string text((char *)p->data);
  *res = op_authenticate(text);
  return status;
}



bool op_sameHashes(unsigned char *h1, unsigned char *h2) {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
    if (h1[i] != h2[i]) { return false; }
  }
  return true;
}


bool op_verify(std::string text, auth_t a) {
  unsigned char hash[SHA256_DIGEST_LENGTH];
  std::string s = std::to_string(a.id) + OPsecret + text;

  if (!SHA256 ((const unsigned char *)s.c_str(), s.size(), hash)) { }

  return op_sameHashes(a.hash.hash,hash);
}


bool op_verifyAuths(std::string text, auths_t auths) {
  for (int i = 0; i < auths.size; i++) {
    if (!op_verify(text,auths.auths[i])) { return false; }
  }
  return true;
}


sgx_status_t OP_TEEverify(payload_t *p, auths_t *a, bool *res) {
  sgx_status_t status = SGX_SUCCESS;
  std::string text;
  for (int i = 0; i < p->size; i++) { text += (p->data)[i]; }
  *res = op_verifyAuths(text, *a);
  return status;
}


// From EnclaveShare.cpp
std::string op_hash2str(hash_t hash) {
  std::string text;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { text += hash.hash[i]; }
  text += std::to_string(hash.set);
  return text;
}


std::string op_H2S(hash_t hash) {
  std::string text;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { text += std::to_string((int)hash.hash[i]); }
  return text;
}


sgx_status_t OP_TEEprepare(hash_t *hash, opproposal_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  if (OPph == OPpa) {
    OPph = OPpb;
    std::string sh = op_hash2str(*hash);
    std::string text = sh + std::to_string(OPview);
    //ocall_print(("ENCLAVE:preparing-hash(" + std::to_string(sh.size()) + "):-" + sh + "-").c_str());
    //ocall_print(("ENCLAVE:preparing-hash:-" + H2S(*hash) + "-").c_str());
    //ocall_print(("ENCLAVE:preparing-view:" + std::to_string(OPview)).c_str());
    res->hash = *hash;
    res->view = OPview;
    res->auth = op_authenticate(text);
  } else { res->auth.hash.set = false; }

  return status;
}


sgx_status_t OP_TEEvote(hash_t *hash, opvote_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  std::string sh = op_hash2str(*hash);
  std::string text = sh + std::to_string(OPview);
  //ocall_print(("ENCLAVE:preparing-hash(" + std::to_string(sh.size()) + "):-" + sh + "-").c_str());
  //ocall_print(("ENCLAVE:preparing-hash:-" + H2S(*hash) + "-").c_str());
  //ocall_print(("ENCLAVE:preparing-view:" + std::to_string(OPview)).c_str());
  res->hash = *hash;
  res->view = OPview;
  res->auths.size = 1;
  res->auths.auths[0] = op_authenticate(text);

  return status;
}


bool op_verifyOrInit(opproposal_t *just) {
  // We don't verify the initial justification, whch is a dummy one
  if (OPview == 0 && OPprepv == 0 && just->view == 0 && just->hash.set == false) {
    return true;
  }
  std::string text = op_hash2str(just->hash) + std::to_string(just->view);
  return op_verify(text,just->auth);
}


sgx_status_t OP_TEEstore(opproposal_t *just, opstore_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  // TODO: check that just is from the leader
  if (op_verifyOrInit(just) && OPview >= just->view && just->view >= OPprepv) {
    //ocall_print(("ENCLAVE:storing" + std::to_string(OPid)).c_str());
    OPprepv = just->view;
    res->view = OPview;
    res->hash = just->hash;
    res->v = just->view;
    res->auth = op_authenticate(std::to_string(OPview) + op_hash2str(just->hash) + std::to_string(just->view));
    OPview++;
    OPph = OPpa;
    //ocall_print(("ENCLAVE:stored with auth (" + std::to_string(res->auth2.id) + ")" + std::to_string(OPid)).c_str());
    //ocall_print(("ENCLAVE:stored" + std::to_string(OPid)).c_str());
  } else { res->auth.hash.set=false; }

  return status;
}


/*
sgx_status_t OP_TEEaccum(fjust_t *j, fjusts_t *js, hash_t *prp, haccum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  View v = j->data.view;
  View highest = j->data.justv;
  hash_t hash = j->data.justh;
  std::set<PID> signers;
  signers.insert(j->auth.id);

  for (int i = 0; i < MAX_NUM_SIGNATURES-1 && i < OPqsize-1; i++) {
    fjust_t just   = js->justs[i];
    fdata_t data   = just.data;
    auth_t  auth   = just.auth;
    PID     signer = auth.id;
    std::string text = std::to_string(data.justv) + op_hash2str(data.justh) + std::to_string(data.view);
    if (data.view == v
        && signers.find(signer) == signers.end()
        && op_verify(text,auth)
        && highest >= data.justv) {
      signers.insert(signer);
    }
  }

  unsigned int size = signers.size();
  //ocall_print(("ENCLAVE:TEEaccum" + std::to_string(size)).c_str());
  std::string text = std::to_string(true) + std::to_string(v) + std::to_string(highest) + op_hash2str(hash) + std::to_string(size);
  res->auth = op_authenticate(text);
  res->set = true;
  res->view = v;
  res->prepv = highest;
  res->hash = hash;
  res->size = size;

  // prepare part
  if (OPphase == PH2B && v == OPview) {
    std::string sh = op_hash2str(*prp);
    std::string textp = sh + std::to_string(OPview);
    res->authp = op_authenticate(textp);
  } else { res->set = false; }

  return status;
}


sgx_status_t OP_TEEaccumSp(ofjust_t *just, hash_t *prp, haccum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  fdata_t data  = just->data;
  auths_t auths = just->auths;
  View    view  = data.view;
  hash_t  justh = data.justh;
  View    justv = data.justv;

  std::set<PID> signers;
  std::string text = std::to_string(justv) + op_hash2str(justh) + std::to_string(view);

  for (int i = 0; i < MAX_NUM_SIGNATURES && i < OPqsize && i < auths.size; i++) {
    auth_t auth = auths.auths[i];
    PID signer = auth.id;
    if (signers.find(signer) == signers.end() && op_verify(text,auth)) { signers.insert(signer); }
  }

  unsigned int size = signers.size();
  //ocall_print(("ENCLAVE:OP_TEEaccumSp:" + std::to_string(auths.size) + ":" + std::to_string(size)).c_str());
  std::string atext = std::to_string(true) + std::to_string(view) + std::to_string(justv) + op_hash2str(justh) + std::to_string(size);
  res->auth = op_authenticate(atext);
  res->set = true;
  res->view = view;
  res->prepv = justv;
  res->hash = justh;
  res->size = size;

  // prepare part
  if (OPphase == PH2B && view == OPview) {
    std::string sh = op_hash2str(*prp);
    std::string textp = sh + std::to_string(OPview);
    res->authp = op_authenticate(textp);
  } else { res->set = false; }

  return status;
}
*/
