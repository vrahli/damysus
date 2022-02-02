#include <set>
#include "EnclaveShare.h"


cblock_t GENblock = new_cblock_t();
//hash_t CHCOMBpreph = newHash();     // hash of the last prepared block
hash_t CHCOMBpreph = hashCBlock(&GENblock);     // hash of the last prepared block
View   CHCOMBprepv = 0;             // preph's view
View   CHCOMBview  = 0;             // current view
Phase1 CHCOMBphase = PH1_NEWVIEW;   // current phase



// increments the (view,phase) pair
void CHCOMBincrement() {
  if (CHCOMBphase == PH1_PREPARE) {
    CHCOMBphase = PH1_NEWVIEW;
  } else if (CHCOMBphase == PH1_NEWVIEW) {
    CHCOMBphase = PH1_PREPARE;
    CHCOMBview++;
  }
}


just_t CHCOMBsign(hash_t h1, hash_t h2, View v2) {
  rdata_t rdata;
  rdata.proph = h1; rdata.propv = CHCOMBview; rdata.justh = h2; rdata.justv = v2; rdata.phase = CHCOMBphase;
  sign_t sign = signString(rdata2string(rdata));
  signs_t signs; signs.size = 1; signs.signs[0] = sign;
  just_t j; j.set = 1; j.rdata = rdata; j.signs = signs;

  CHCOMBincrement();

  return j;
}


sgx_status_t CH_COMB_TEEsign(just_t *just) {
  sgx_status_t status = SGX_SUCCESS;
  hash_t hash = noHash();

  //if (CHCOMBview == 0 && CHCOMBphase == PH1_NEWVIEW) { CHCOMBpreph = hashCBlock(&GENblock); }

  *just = CHCOMBsign(hash,CHCOMBpreph,CHCOMBprepv);

  return status;
}


sgx_status_t CH_COMB_TEEverify(just_t *just, unsigned int *res) {
  sgx_status_t status = SGX_SUCCESS;
  bool b = verifyJust(just);
  *res=b;
  return status;
}


sgx_status_t CH_COMB_TEEprepare(cblock_t *block, hash_t *hash, just_t *res) {
  //ocall_print("TEEprepare...");
  sgx_status_t status = SGX_SUCCESS;

  ca_t cert = block->cert;
  bool vb = verifyCA(cert);
  bool vv = CHCOMBview == ca2cview(cert)+1;

  //ocall_print((nfo() + "TEEprepare " + std::to_string(block->view) + " " + std::to_string(ca2view(cert))).c_str());

  if (vb && vv) {

    if (eqHashes(ca2hash(cert),*hash)) {
      CHCOMBpreph = ca2hash(cert);
      CHCOMBprepv = ca2view(cert);
    } else {
      ocall_print((nfo() + "TEEprepare - different hashes " + std::to_string(block->view) + " " + std::to_string(ca2view(cert))).c_str());
      ocall_print((nfo() + "hash1:" + hash2string(ca2hash(cert))).c_str());
      ocall_print((nfo() + "hash2:" + hash2string(*hash)).c_str());
    }

    //ocall_print((nfo() + "block to hash:" + cblock2string(block)).c_str());
    //ocall_print((nfo() + "block to CAtag:" + std::to_string((block->cert).tag)).c_str());
    //ocall_print((nfo() + "block to hash (ca 1st sign):" + sign2string(getN(ca2signs(block->cert)))).c_str());
    hash_t hash2 = hashCBlock(block);
    //ocall_print((nfo() + "new hash:" + hash2string(hash2)).c_str());
    *res = CHCOMBsign(hash2,newHash(),0);
  } else {
    ocall_print(("TEEprepare failed " + std::to_string(vb) + " " + std::to_string(vv)).c_str());
    res->set=false;
  }

  return status;
}


sgx_status_t CH_COMB_TEEaccum(onejusts_t *js, accum_t *res) {
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


sgx_status_t CH_COMB_TEEaccumSp(just_t *just, accum_t *res) {
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
