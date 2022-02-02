//#include <algorithm>

#include "Sign.h"


/*std::ostream& operator<<(std::ostream& os, const Sign& s) {
  for (std::array<unsigned char,SIGN_LEN>::const_iterator it=s.sign.begin(); it!=s.sign.end(); ++it) {
    os << *it;
  }
  return os;
}*/

bool Sign::operator<(const Sign& s) const {
  if (signer < s.signer) { return true; }
  return false;
}



void Sign::serialize(salticidae::DataStream &data) const {
  data << this->set << this->signer;
  for (int i = 0; i < SIGN_LEN; i++) { data << this->sign[i]; }
}


void Sign::unserialize(salticidae::DataStream &data) {
  data >> this->set >> this->signer;
  for (int i = 0; i < SIGN_LEN; i++) { data >> this->sign[i]; }
}


Sign::Sign() {
  for (int i = 0; i < SIGN_LEN; i++) { this->sign[i] = '0'; }
}

Sign::Sign(PID signer, char c) {
  this->set=true;
  this->signer=signer;
  for(int i = 0; i < SIGN_LEN; i++) { (this->sign)[i] = c; }
}

Sign::Sign(PID signer, unsigned char s[SIGN_LEN]) {
  this->set=true;
  this->signer=signer;
  for(int i = 0; i < SIGN_LEN; i++) { (this->sign)[i] = s[i]; }
}

Sign::Sign(bool b, PID signer, unsigned char s[SIGN_LEN]) {
  this->set=b;
  this->signer=signer;
  for(int i = 0; i < SIGN_LEN; i++) { (this->sign)[i] = s[i]; }
}

static void printHex(const char *title, const unsigned char *s, int len) {
  int n;
  printf("%s:", title);
  for (n = 0; n < len; ++n) {
    if ((n % 16) == 0) {
      printf("\n%04x", n);
    }
    printf(" %02x", s[n]);
  }
  printf("\n");
}

void signText(std::string text, RSA_key priv, unsigned char sign[SIGN_LEN]) {
  unsigned int signLen;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  if (!RSA_sign (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &signLen, priv)) {
    std::cout << KCYN << "RSA_sign failed" << KNRM << std::endl;
    exit(0);
  }
}


void old_signText(std::string text, EC_key priv, unsigned char sign[SIGN_LEN]) {
  unsigned int signLen;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  if (!ECDSA_sign (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &signLen, priv)) {
    std::cout << KCYN << "ECDSA_sign failed" << KNRM << std::endl;
    exit(0);
  }
}



void signText(std::string text, EC_key priv, unsigned char sign[SIGN_LEN]) {
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
  }

  unsigned int signLen = ECDSA_size(priv);
  //unsigned int signLen = len;

  unsigned char *buf = NULL;
  if ((buf = (unsigned char *)OPENSSL_malloc(signLen)) == NULL) {
    printf("malloc failed");
  }
  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++) { buf[k]='0'; }

  if (!ECDSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, buf, &signLen, priv)) {
    printf(("ECDSA_sign failed(len=" + std::to_string(signLen) + ")").c_str());
  }

  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++) { sign[k]=buf[k]; }
  OPENSSL_free(buf);
}


void signText(std::string text, EVP_key priv, unsigned char sign[SIGN_LEN]) {
  size_t signLen = 0;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  // setting up context
  EVP_PKEY_CTX * key_ctx = EVP_PKEY_CTX_new(priv,NULL);
  EVP_PKEY_sign_init(key_ctx);
  EVP_PKEY_CTX_set_signature_md(key_ctx, EVP_sha256());

  // Signing
  if (!EVP_PKEY_sign(key_ctx, NULL, &signLen, hash, SHA256_DIGEST_LENGTH)) {
    std::cout << KCYN << "signature(0) failed" << KNRM << std::endl;
  }
  //std::cout << KCYN << "signature(0) length:" << signLen << KNRM << std::endl;
  if (!EVP_PKEY_sign(key_ctx, sign, &signLen, hash, SHA256_DIGEST_LENGTH)) {
    std::cout << KCYN << "signature(1) failed" << KNRM << std::endl;
  }
  //std::cout << KCYN << "signature length:" << signLen << KNRM << std::endl;
  EVP_PKEY_CTX_free(key_ctx);
}


bool verifyText(std::string text, RSA_key pub, unsigned char sign[SIGN_LEN]) {
  size_t signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  bool b = RSA_verify (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, signLen, pub);

  return b;
}


bool verifyText(std::string text, EC_key pub, unsigned char sign[SIGN_LEN]) {
  if (DEBUG) { std::cout << KCYN << "verifying text using EC" << KNRM << std::endl; }
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  bool b = ECDSA_verify (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, signLen, pub);

  return b;
}


bool verifyText(std::string text, EVP_key pub, unsigned char sign[SIGN_LEN]) {
  if (DEBUG) { std::cout << KCYN << "verifying text using EC" << KNRM << std::endl; }
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  // setting up context
  EVP_PKEY_CTX * key_ctx = EVP_PKEY_CTX_new(pub,NULL);
  EVP_PKEY_verify_init(key_ctx);
  EVP_PKEY_CTX_set_signature_md(key_ctx, EVP_sha256());

  bool b=EVP_PKEY_verify(key_ctx, sign, signLen, hash, SHA256_DIGEST_LENGTH);
  EVP_PKEY_CTX_free(key_ctx);

  return b;
}


Sign::Sign(KEY priv, PID signer, std::string text) {
  this->set=true;
  this->signer=signer;
  signText(text,priv,this->sign);
}

std::string Sign::toString() {
  std::string s = std::to_string(this->set) + std::to_string(this->signer);
  for (int i = 0; i < SIGN_LEN; i++) { s += (this->sign)[i]; }
  return s;
}

std::string Sign::prettyPrint() {
  return "S[" + std::to_string(this->set) + "," + std::to_string(this->signer) + "]";
}


PID Sign::getSigner() { return this->signer; }

unsigned char * Sign::getSign() { return this->sign; }

bool Sign::isSet() { return this->set; }

bool Sign::verify(KEY pub, std::string text) {
  bool b = verifyText(text,pub,this->sign);
  if (DEBUG) { std::cout << KCYN << "verified signature: " << b << KNRM << std::endl; }
  return b;
}

bool Sign::operator==(const Sign& s) const {
  if (this->set != s.set || this->signer != s.signer) { return false; }
  for (int i = 0; i < SIGN_LEN; i++) { if (this->sign[i] != s.sign[i]) { return false; } }
  return true;
}
