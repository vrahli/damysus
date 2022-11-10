#include <set>
#include "EnclaveShare.h"



bool verifyCData(cdata_t data, sign_t sign) {
  signs_t signs; signs.size = 1; signs.signs[0] = sign;
  return verifyText(signs,cdata2string(data));
}


bool verifyCert(cert_t c) {
  std::string text = std::to_string(PH1_PREPARE) + std::to_string(c.view) + hash2string(c.hash);
  return verifyText(c.signs,text);
}


struct vote_compare {
  bool operator()(vote_t v1, vote_t v2) const {
    return ((v1.cdata.cert.view < v2.cdata.cert.view)
            || ((v1.cdata.cert.view == v2.cdata.cert.view)
                && (v1.sign.signer < v2.sign.signer)));
  }
};


// Compared to old_TEEaccum, here we don't verify certificates if we don't want to return
// the corresponding hash value
sgx_status_t TEEaccum(votes_t *vs, accum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  View v = vs->votes[0].cdata.view;

  std::set<PID> signers;
  std::set<vote_t,vote_compare> ordVotes;
  if (DEBUG1) ocall_print((nfo() + "max_num_sign=" + std::to_string(MAX_NUM_SIGNATURES) + ";qsize=" + std::to_string(getQsize())).c_str());
  for (int i = 0; i < MAX_NUM_SIGNATURES && i < getQsize(); i++) {
    if (DEBUG1) ocall_print((nfo() + "TEEaccum[" + std::to_string(i) + "]").c_str());
    vote_t vote = vs->votes[i];
    cdata_t data = vote.cdata;
    sign_t sign = vote.sign;
    PID signer = sign.signer;
    if (data.phase == PH1_NEWVIEW
        && data.view == v
        && signers.find(signer) == signers.end()
        && verifyCData(data,sign)) {
      signers.insert(signer);
      ordVotes.insert(vs->votes[i]);
    }
  }

  View highestView = 0;
  hash_t highestHash;

  bool found = false;
  unsigned int size = signers.size();
  for (std::set<vote_t,vote_compare>::iterator it=ordVotes.begin(); it!=ordVotes.end() && !found; ++it) {
    vote_t vote = (vote_t)*it;
    cert_t cert = vote.cdata.cert;
    if (verifyCert(cert)) {
      found = true;
      highestView = cert.view;
      highestHash = cert.hash;
    } else { size--; }
  }

  bool set = true;
  std::string text = std::to_string(set) + std::to_string(v) + std::to_string(highestView) + hash2string(highestHash) + std::to_string(size);
  sign_t sign = signString(text);
  res->set = 1;
  res->view = v;
  res->prepv = highestView;
  res->hash = highestHash;
  res->size = size;
  res->sign = sign;

  return status;
}


sgx_status_t TEEaccumSp(uvote_t *vote, accum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  cdata_t cdata = vote->cdata;
  signs_t ss = vote->signs;
  // extracted from cdata:
  Phase1 p = cdata.phase;
  View v = cdata.view;
  cert_t cert = cdata.cert;
  // extracted from c:
  View highest = cert.view;
  hash_t hash = cert.hash;

  std::set<PID> signers;

  bool vc = verifyCert(cert);
  if (p == PH1_NEWVIEW && vc) {
    for (int i = 0; i < MAX_NUM_SIGNATURES && i < getQsize() && i < ss.size; i++) {
      sign_t sign = ss.signs[i];
      PID signer = sign.signer;
      bool vd = verifyCData(cdata,sign);
      if (vd && signers.find(signer) == signers.end()) { signers.insert(signer); }
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


/*
sgx_status_t old_TEEaccum(votes_t *vs, accum_t *res) {
  sgx_status_t status = SGX_SUCCESS;

  PreVote votes[MAX_NUM_SIGNATURES];
  getVotes(*vs,votes);
  View v = votes[0].getCData().getView();
  View highestView = 0;
  PreHash highestHash;
  std::set<PID> signers;

  for (int i = 0; i < MAX_NUM_SIGNATURES && i < getQsize(); i++) {
    PreCData data = votes[i].getCData();
    PreSign sign = votes[i].getSign();
    PreCert cert = data.getCert();
    PID signer = sign.getSigner();
    if (data.getPhase() == PH1_NEWVIEW
        && data.getView() == v
        && signers.find(signer) == signers.end()
        && verifyCData(data,sign)
        && verifyCert(cert)) {
      if (DEBUG1) ocall_print((nfo() + "inserting signer (" + std::to_string(signer) + ")").c_str());
      signers.insert(signer);
      if (cert.getView() >= highestView) {
        highestView = cert.getView();
        highestHash = cert.getHash();
      }
    } else {
      if (DEBUG1) ocall_print((nfo() + "not inserting signer (" + std::to_string(signer) + ")").c_str());
      if (DEBUG1) ocall_print((nfo() + "vote:" + votes[i].prettyPrint()).c_str());
    }
  }

  unsigned int size = signers.size();
  std::string text = std::to_string(v) + std::to_string(highestView) + highestHash.toString() + std::to_string(size);
  PreSign sign = signText(text);
  PreAccum acc(v,highestView,highestHash,size,sign);
  setAccum(acc,res);

  return status;
}
*/
