#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>

#include "../Enclave/user_types.h"


// From key.h
/* const char priv_key256[] = { */
/*   "-----BEGIN PRIVATE KEY-----\n" */
/*   "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgnI0T6AoPs+ufh54e\n" */
/*   "3tr6ywY7KkMBZhBs69NvMpvtXeehRANCAAS+G04ABpuwCvaS0v5fi9vuNOEitPon\n" */
/*   "4nIDK/IJOsGXv85Jw5wayZI19lSB6ox05rLB+CxmEXrDyiOhX8Sz7c0L\n" */
/*   "-----END PRIVATE KEY-----" */
/* /\*  "-----BEGIN PRIVATE KEY-----\n" */
/*   "IGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgWoH9Crif3RmM9vK8\n" */
/*   "g3uftiBG3fk/6CVZIjfmaAhH+ChRANCAASLnxAnxhPA+XIJEdJCVB/PMr9RIVpL\n" */
/*   "zd8iNpQTmlGPtrgUgz85P3I5GNsLUejkPqzgQL/yR/LEZWMjy5DPvPN\n" */
/*   "----END PRIVATE KEY-----"*\/ */
/* }; */

/* const char pub_key256[] = { */
/*   "-----BEGIN PUBLIC KEY-----\n" */
/*   "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEvhtOAAabsAr2ktL+X4vb7jThIrT6\n" */
/*   "J+JyAyvyCTrBl7/OScOcGsmSNfZUgeqMdOaywfgsZhF6w8ojoV/Es+3NCw==\n" */
/*   "-----END PUBLIC KEY-----" */
/* /\*  "-----BEGIN PUBLIC KEY-----\n" */
/*   "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEi58QJ8YTwPlyCRHSQlQfzzK/USFa\n" */
/*   "S683fIjaUE5pRj7a4FIM/OT9yORjbC1Ho5D6s4EC/8kfyxGVjI8uQz7zzQ==\n" */
/*   "-----END PUBLIC KEY-----"*\/ */
/* }; */



hash_t lockh;                 // hash of the last locked block
View   lockv = 0;             // lockh's view
hash_t preph;                 // hash of the last prepared block
View   prepv = 0;             // preph's view
View   view  = 0;             // current view
Phase1 phase = PH1_NEWVIEW;   // current phase
PID    id    = 0;             // unique identifier
KEY    priv;                  // private key
KEY    pub;                   // public key



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


bool verifyText(char *text, unsigned int len, EC_key pub, unsigned char sign[SIGN_LEN]) {
  //printf("verifying text using EC\n");
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text, len, hash)){
    printf("SHA1 failed\n");
    exit(0);
  }

  bool b = ECDSA_verify (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, signLen, pub);
  return b;
}


// sign is the output
void signData(sign_t *sign, KEY priv, PID signer, char *text, unsigned int len) {
  sign->set=true;
  sign->signer=signer;

  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text, len, hash)){
    printf("hashing failed\n");
  }

  unsigned int signLen = ECDSA_size(priv);
  //unsigned int signLen = len;

  unsigned char *buf = NULL;
  if ((buf = (unsigned char *)OPENSSL_malloc(signLen)) == NULL) {
    printf("malloc failed\n");
  }
  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++) { buf[k]='0'; }

  if (!ECDSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, buf, &signLen, priv)) {
    printf("ECDSA_sign failed(len=%d)\n", signLen);
  }

  for (int k = 0; k < SHA256_DIGEST_LENGTH; k++) { sign->sign[k]=buf[k]; }
  OPENSSL_free(buf);
}


// data is the output
void rdata2string(unsigned char *data, rdata_t d) {
  *data = *d.proph.hash;
  *(data + SHA256_DIGEST_LENGTH) = d.proph.set;
  *(data + sizeof(hash_t)) = d.propv;
  *(data + sizeof(hash_t) + sizeof(View)) = *d.justh.hash;
  *(data + sizeof(hash_t) + sizeof(View) + SHA256_DIGEST_LENGTH) = d.justh.set;
  *(data + 2*sizeof(hash_t) + sizeof(View)) = d.justv;
  *(data + 2*sizeof(hash_t) + 2*sizeof(View)) = d.phase;
}


// j is the output
void justify(just_t *j, hash_t h1, hash_t h2, View v2) {
  j->set = true;

  j->rdata.proph = h1;
  j->rdata.propv = view;
  j->rdata.justh = h2;
  j->rdata.justv = v2;
  j->rdata.phase = phase;

  j->signs.size = 1;

  unsigned char data[sizeof(rdata_t)];
  rdata2string(data,j->rdata);

  signData(&((j->signs.signs)[0]),priv,id,data,sizeof(rdata_t));

  increment();
}


bool checkJust(just_t *j) {
  unsigned char data[sizeof(rdata_t)];
  rdata2string(data,j->rdata);
  return verifyText(data, sizeof(rdata_t), pub, (j->signs.signs)[0].sign);
}


// j is the output
void TEEsign(just_t *j) {
  hash_t *h = (hash_t *)malloc(sizeof(hash_t));
  h->set = false;
  justify(j,*h,preph,prepv);
}


int main(int argc, char *argv[]) {
  // setting private key
  BIO *bio1 = BIO_new(BIO_s_mem());
  int w1 = BIO_write(bio1,priv_key256,sizeof(priv_key256));
  priv = PEM_read_bio_ECPrivateKey(bio1, NULL, NULL, NULL);

  // setting public key
  BIO *bio2 = BIO_new(BIO_s_mem());
  int w2 = BIO_write(bio2,pub_key256,sizeof(pub_key256));
  pub = PEM_read_bio_EC_PUBKEY(bio2, NULL, NULL, NULL);

  // signing
  just_t *j = (just_t *)malloc(sizeof(just_t));
  TEEsign(j);

  // checking signature
  bool b = checkJust(j);
  printf("verified? %s\n", b ? "true" : "false");
}



// Compile: gcc App/TrustedFunC.c -x c -o checker -lssl -lcrypto
// run: ./checker
// expected output: "verified? true"
