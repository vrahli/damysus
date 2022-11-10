#include "EnclaveShare.h"


PID     id;             // unique identifier
KEY     priv;           // private key
unsigned int qsize;     // quorum size
std::map<PID,KEY> pubs; // public keys

double cryptoTime;


std::string nfo() { return "[" + std::to_string(id) + "]"; }


void loadPrivateKey() {
  BIO *bio = BIO_new(BIO_s_mem());
  int w = BIO_write(bio,priv_key256,sizeof(priv_key256));
  //ocall_print(("wrote:"+std::to_string(w)).c_str());
  priv = PEM_read_bio_ECPrivateKey(bio, NULL, NULL, NULL);
  if (!EC_KEY_check_key(priv)) {
    ocall_print((nfo() + "priv key doesn't check").c_str());
  }
  ocall_print((nfo() + "ENCLAVE:loaded private key").c_str());
  //ecdsa_test();
  //testSign();
}


sgx_status_t initialize_variables(PID *me, pids_t *others, unsigned int *q) {
  sgx_status_t status = SGX_SUCCESS;

  // Initialize SGXSSL crypto
  //OPENSSL_init_crypto(0, NULL);

  //sgx_device_status_t result;
  //status = sgx_cap_enable_device(&result);

  id = *me;
  ocall_print((nfo() + "ENCLAVE:set up id-" + std::to_string(id)).c_str());

  qsize = *q;
  //ocall_print((nfo() + "ENCLAVE:set up the quorum size-" + std::to_string(qsize)).c_str());

  if (DEBUG1) ocall_print((nfo() + "qsize=" + std::to_string(qsize)).c_str());

  // Doesn't appear to work -- for now everyone will use the same keys
  //ocall_load_private_key(&id,&priv);

  loadPrivateKey();

  for (int i = 0; i < others->num_nodes; i++) {
    PID j    = others->pids[i];
    BIO *bio = BIO_new(BIO_s_mem());
    int w    = BIO_write(bio,pub_key256,sizeof(pub_key256));
    KEY pub  = PEM_read_bio_EC_PUBKEY(bio, NULL, NULL, NULL);
    if (!EC_KEY_check_key(pub)) {ocall_print((nfo() + "pub key doesn't check").c_str()); }
    // Doesn't appear to work
    //ocall_load_public_key(&j,&pub);
    pubs[j]=pub;
  }
  ocall_print((nfo() + "ENCLAVE:loaded public keys:" + std::to_string(0) + "-" + std::to_string(others->num_nodes)).c_str());

  return status;
}


bool verifyBuf(std::string text, EC_key pub, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "verifying text using EC" << KNRM << std::endl; }
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    //std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    //exit(0);
  }

  bool b = ECDSA_verify (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, signLen, pub);

  return b;
}



bool verifySign(sign_t s, KEY pub, std::string text) {
  if (DEBUGOT) ocall_setCtime();
  unsigned char *sign = &s.sign[0];
  bool b = verifyBuf(text,pub,sign);
  if (DEBUGOT) ocall_recCVtime();
  //if (DEBUG) { std::cout << KCYN << "verified signature: " << b << KNRM << std::endl; }
  return b;
}


bool verifySigns(signs_t signs, PID id, std::map<PID,KEY> pubs, std::string s) {
  for (int i = 0; i < signs.size; i++) {
    sign_t sign = signs.signs[i];
    PID node = sign.signer;
    if (true) { //(id != node) { // we don't check our own signature
      std::map<PID,KEY>::iterator it = pubs.find(node);
      if (it != pubs.end()) {
        // TODO: The id of the signer should also be added to the string (for signing and verifying---maybe in Sign.cpp instead?)
        //ocall_print (("verifying signature from:" + std::to_string(node)).c_str());
        if (!verifySign(sign,it->second,s)) {
          ocall_print (("cannot very signature from:" + std::to_string(node)).c_str());
          return false;
        }
      } else {
        ocall_print (("unknown node:" + std::to_string(node)).c_str());
        return false;
      }
    }
  }
  return true;
}


bool verifyText(signs_t signs, std::string text) {
  //ocall_print((nfo() + "verifying:" + std::to_string(signs.size)).c_str());
  bool b = verifySigns(signs,id,pubs,text);
  //auto end = std::chrono::steady_clock::now();
  //double time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
  //cryptoTime += time;
  return b;
}


std::string hash2string(hash_t hash) {
  std::string text;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { text += hash.hash[i]; }
  text += std::to_string(hash.set);
  return text;
}


std::string rdata2string(rdata_t rdata) {
  return (hash2string(rdata.proph)
          + std::to_string(rdata.propv)
          + hash2string(rdata.justh)
          + std::to_string(rdata.justv)
          + std::to_string(rdata.phase));
}


std::string sign2string(sign_t sign) {
  std::string s = std::to_string(sign.set) + std::to_string(sign.signer);
  for (int i = 0; i < SIGN_LEN; i++) { s += sign.sign[i]; }
  return s;
}


std::string signs2string(signs_t signs) {
  std::string text = std::to_string(signs.size);
  for (int i = 0; i < signs.size; i++) {
    text += sign2string(signs.signs[i]);
  }
  return text;
}


std::string cert2string(cert_t cert) {
  return (std::to_string(cert.view)
          + hash2string(cert.hash)
          + signs2string(cert.signs));
}


std::string cdata2string(cdata_t cdata) {
  return (std::to_string(cdata.phase)
          + std::to_string(cdata.view)
          + cert2string(cdata.cert));
}


std::string accum_data2string(accum_t acc) {
  return (std::to_string(acc.set)
          + std::to_string(acc.view)
          + std::to_string(acc.prepv)
          + hash2string(acc.hash)
          + std::to_string(acc.size));
}


std::string accum2string(accum_t acc) {
  return (accum_data2string(acc) + sign2string(acc.sign));
}


std::string just2string(just_t just) {
  return std::to_string(just.set) + rdata2string(just.rdata) + signs2string(just.signs);
}


std::string transaction2string(trans_t trans) {
  std::string text = std::to_string(trans.clientid) + std::to_string(trans.transid);
  for (int i = 0; i < PAYLOAD_SIZE; i++) {
    text += std::to_string(trans.data[i]);
  }
  return text;
}


std::string transactions2string(unsigned int size, trans_t *trans) {
  std::string text;
  for (int i = 0; i < size; i++) {
    text += transaction2string(trans[i]);
  }
  return text;
}


std::string jblock2string(jblock_t *block) {
  return (std::to_string(block->set)
//          + std::to_string(block->executed)
          + std::to_string(block->view)
          + just2string(block->just)
          + std::to_string(block->size)
          + transactions2string(block->size,block->trans));
}

std::string ca2string(ca_t ca) {
  if (ca.tag == CERT) { return cert2string(ca.cert); }
  return accum2string(ca.accum);
}

std::string cblock2string(cblock_t *block) {
  return (std::to_string(block->set)
//          + std::to_string(block->executed)
          + std::to_string(block->view)
          + ca2string(block->cert)
          + std::to_string(block->size)
          + transactions2string(block->size,block->trans));
}


bool verifyAccum(accum_t *acc) {
  signs_t signs; signs.size = 1; signs.signs[0] = acc->sign;
  return verifyText(signs,accum_data2string(*acc));
}


bool verifyJust(just_t *just) {
  return verifyText(just->signs,rdata2string(just->rdata));
}


unsigned int getQsize() {
  return qsize;
}



bool signBuf(PID id, std::string text, EC_key priv, unsigned char sign[SIGN_LEN]) {
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    return false;
  }

  unsigned int signLen = ECDSA_size(priv);
  //unsigned int signLen = len;

  unsigned char *buf = NULL;
  if ((buf = (unsigned char *)OPENSSL_malloc(signLen)) == NULL) {
    ocall_print("malloc failed");
  }
  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++) { buf[k]='0'; }

  if (!ECDSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, buf, &signLen, priv)) {
    ocall_print(("[" + std::to_string(id) + "]ECDSA_sign failed(len=" + std::to_string(signLen) + ")").c_str());
    return false;
  }

  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++) { sign[k]=buf[k]; }
  OPENSSL_free(buf);

  return true;
}


sign_t sign(KEY priv, PID signer, std::string text) {
  sign_t sign;
  sign.signer=signer;
  unsigned char s[SIGN_LEN];
  sign.set=signBuf(signer,text,priv,s);
  std::copy(s, s + SIGN_LEN, std::begin(sign.sign));
  return sign;
}


sign_t signString(std::string text) {
  if (DEBUGOT) ocall_setCtime();
  sign_t s = sign(priv,id,text);
  if (DEBUGOT) ocall_recCStime();
  if (!s.set) {
    ocall_print(("[" + std::to_string(id) + "]wrong signature").c_str());
  }
  return s;
}


bool eqHashes(hash_t h1, hash_t h2) {
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { if (h1.hash[i] != h2.hash[i]) { return false; } }
  return true;
}

hash_t newHash() {
  hash_t hash;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { hash.hash[i] = '0'; }
  hash.set=true;
  return hash;
}


hash_t noHash() {
  hash_t hash;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { hash.hash[i] = '0'; }
  hash.set = false;
  return hash;
}


// The initial justification
bool wellFormedInit(just_t just) {
  return (just.rdata.propv == 0
          && just.rdata.justv == 0
          && just.rdata.phase == PH1_NEWVIEW
          && just.signs.size == 0);
}


bool wellFormedNv(just_t just) {
  return (just.rdata.justh.set
          && just.rdata.phase == PH1_NEWVIEW
          && just.signs.size == 1);
}


bool wellFormedPrep(just_t just) {
  return (just.rdata.proph.set
          && just.rdata.phase == PH1_PREPARE
          && just.signs.size == qsize);
}


// In the chained version there are 2 kinds of justifications
// (those created out of prepare messages, and those sent in new-view messages)
bool WF(just_t just) {
  return (wellFormedInit(just) || wellFormedNv(just) || wellFormedPrep(just));
}


// The view at which the certificate was generated depends on the kind of certificate we have
View just2cview(just_t just) {
  if (wellFormedInit(just) || wellFormedNv(just)) { return just.rdata.justv; }
  // othersize it must be wellFormedPrep()
  return just.rdata.propv;
}


hash_t just2chash(just_t just) {
  if (wellFormedInit(just) || wellFormedNv(just)) { return just.rdata.justh; }
  // otherwise it must be wellFormedPred(qsize)
  return just.rdata.proph;
}


hash_t hashJBlock(jblock_t *block) {
  hash_t h;
  h.set = true;
  std::string text = jblock2string(block);
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { h.hash[i] = '0'; }

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), h.hash)){
    //std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    //exit(0);
  }

  return h;
}


hash_t hashCBlock(cblock_t *block) {
  hash_t h;
  h.set = true;
  std::string text = cblock2string(block);
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { h.hash[i] = '0'; }

  //ocall_print(("hashing:" + text).c_str());

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), h.hash)){
    //std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    //exit(0);
  }

  return h;
}



hash_t ca2hash(ca_t ca) {
  if (ca.tag == CERT) { return ca.cert.hash; }
  return ca.accum.hash;
}


View ca2view(ca_t ca) {
  if (ca.tag == CERT) { return ca.cert.view; }
  return ca.accum.prepv;
}


View ca2cview(ca_t ca) {
  if (ca.tag == CERT) { return ca.cert.view; }
  return ca.accum.view;
}

sign_t getN(signs_t signs) {
  return signs.signs[0];
}

signs_t ca2signs(ca_t ca) {
  if (ca.tag == CERT) { return ca.cert.signs; }
  signs_t signs;
  signs.size = 1;
  signs.signs[0] = ca.accum.sign;
  return signs;
}


bool verifyCA(ca_t ca) {
  if (ca.tag == CERT) {
    // We have here to verify the corresponding justification that we generated the certificate from in 'checkNewJustChComb'
    rdata_t rdata;
    rdata.proph = ca.cert.hash;
    rdata.propv = ca.cert.view;
    rdata.justh = noHash();
    rdata.justv = 0;
    rdata.phase = PH1_PREPARE;
    return verifyText(ca.cert.signs,rdata2string(rdata));
  }
  // else
  return verifyAccum(&(ca.accum));
}

hash_t new_hash_t() {
  hash_t hash;
  hash.set = false;
  for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) { hash.hash[i] = '0'; }
  return hash;
}

sign_t new_sign_t() {
  sign_t sign;
  sign.set    = false;
  sign.signer = 0;
  for (int i = 0; i < SIGN_LEN; i++) { sign.sign[i] = '0'; }
  return sign;
}

signs_t new_signs_t() {
  signs_t signs;
  signs.size = 0;
  for (int i = 0; i < MAX_NUM_SIGNATURES; i++) { signs.signs[i] = new_sign_t(); }
  return signs;
}

cert_t new_cert_t() {
  cert_t cert;
  cert.view = 0;
  cert.hash = new_hash_t();
  cert.signs = new_signs_t();
  return cert;
}

trans_t new_trans_t() {
  trans_t trans;
  trans.clientid = 0;
  trans.transid = 0;
  for (int i = 0; i < PAYLOAD_SIZE; i++) { trans.data[i] = '0'; }
  return trans;
}

ca_t new_ca_t() {
  ca_t ca;
  ca.tag = CERT;
  ca.cert = new_cert_t();
  return ca;
}

cblock_t new_cblock_t() {
  cblock_t block;
  block.set = true;
  block.executed = false;
  block.view = 0;
  block.cert = new_ca_t();
  block.size = 0;
  for (int i = 0; i < MAX_NUM_TRANSACTIONS; i++) { block.trans[i] = new_trans_t(); }
  return block;
}
