#include "EnclaveShare.h"


hash_t CHlockh = newHash();     // hash of the last locked block
View   CHlockv = 0;             // lockh's view
hash_t CHpreph = newHash();     // hash of the last prepared block
View   CHprepv = 0;             // preph's view
View   CHview  = 0;             // current view
Phase1 CHphase = PH1_NEWVIEW;   // current phase



// increments the (view,phase) pair
void CHincrement() {
  if (CHphase == PH1_PREPARE) {
    CHphase = PH1_NEWVIEW;
  } else if (CHphase == PH1_NEWVIEW) {
    CHphase = PH1_PREPARE;
    CHview++;
  }
}



just_t CHsign(hash_t h1, hash_t h2, View v2) {
  rdata_t rdata;
  rdata.proph = h1; rdata.propv = CHview; rdata.justh = h2; rdata.justv = v2; rdata.phase = CHphase;
  sign_t sign = signString(rdata2string(rdata));
  signs_t signs; signs.size = 1; signs.signs[0] = sign;
  just_t j; j.set = 1; j.rdata = rdata; j.signs = signs;

  CHincrement();

  return j;
}


sgx_status_t CH_TEEsign(just_t *just) {
  sgx_status_t status = SGX_SUCCESS;
  hash_t hash = noHash();

  *just = CHsign(hash,CHpreph,CHprepv);

  return status;
}


sgx_status_t CH_TEEverify(just_t *just, unsigned int *res) {
  sgx_status_t status = SGX_SUCCESS;
  bool b = verifyJust(just);
  *res=b;
  return status;
}


sgx_status_t CH_TEEprepare(jblock_t *block, jblock_t *block0, jblock_t *block1, just_t *res) {
  //ocall_print("TEEprepare...");
  sgx_status_t status = SGX_SUCCESS;

  just_t just = block->just;
  just_t just0 = block0->just;
  bool vb = verifyJust(&just);
  if (vb
      && CHview == just.rdata.propv+1
      && WF(just)
      && WF(just0)) {

    if (eqHashes(just2chash(just),hashJBlock(block0))) {
      CHpreph = just2chash(just);
      CHprepv = just2cview(just);

      if (eqHashes(just2chash(just0),hashJBlock(block1))) {
        CHlockh = just2chash(just0);
        CHlockv = just2cview(just0);
      }
    }

    // SafeNode check
    if (eqHashes(just2chash(just),CHlockh) || just2cview(just) > CHlockv) {
      *res = CHsign(hashJBlock(block),newHash(),0);
    } else { res->set=false; }
  } else { res->set=false; }

  return status;
}
