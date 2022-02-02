#include <iostream>
#include <fstream>
#include <stdio.h>

#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>
#include <openssl/evp.h>

#include <chrono>
#include <cassert>

#include "KeysFun.h"

#define KNRM  "\x1B[0m"
#define KYEL  "\x1B[49m\x1B[33m"

std::string ldir = "somekeys/";


void signTextRSA(PID id, std::string text, RSA_key priv, unsigned char sign[SIGN_LEN]) {
  if (DEBUG) { std::cout << KCYN << "signing text using EC" << KNRM << std::endl; }
  size_t signLen=0;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  EVP_PKEY * key = EVP_PKEY_new();
  assert(1==EVP_PKEY_assign_RSA(key, priv));

  EVP_PKEY_CTX * key_ctx = EVP_PKEY_CTX_new(key,NULL);
  assert(1==EVP_PKEY_sign_init(key_ctx));
  assert(1==EVP_PKEY_CTX_set_signature_md(key_ctx, EVP_sha256()));

  std::cout << KCYN << "xx" << KNRM << std::endl;

  assert(1==EVP_PKEY_sign(key_ctx,NULL,&signLen, hash, SHA256_DIGEST_LENGTH));
  std::cout << KCYN << "signing text using RSA - len:" << signLen << "-" << SIGN_LEN << KNRM << std::endl;
  //sign.assign(sigLen,0);
  //EVP_MD_CTX * md_ctx = EVP_MD_CTX_new();
  //EVP_MD_CTX_set_pkey_ctx(md_ctx, key_ctx);
  assert(1==EVP_PKEY_sign(key_ctx, sign, &signLen, hash, SHA256_DIGEST_LENGTH));

  EVP_PKEY_CTX_free(key_ctx);
  EVP_PKEY_free(key);
}


void old_signTextRSA(PID id, std::string text, RSA_key priv, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "signing text using RSA" << KNRM << std::endl; }
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

  // printHex("SIGN", sign, signLen);
  //printf("Signature length = %d\n", signLen);

  //std::cout << KCYN << "Result: " << sign << KNRM << std::endl;
}


void signTextEC(PID id, std::string text, EC_key priv, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "signing text using EC" << KNRM << std::endl; }
  size_t signLen=72;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  EVP_PKEY * key = EVP_PKEY_new();
  assert(1==EVP_PKEY_assign_EC_KEY(key, priv));

  EVP_PKEY_CTX * key_ctx = EVP_PKEY_CTX_new(key,NULL);
  assert(1==EVP_PKEY_sign_init(key_ctx));
  assert(1==EVP_PKEY_CTX_set_signature_md(key_ctx, EVP_sha256()));

  //assert(1==EVP_PKEY_sign(key_ctx,NULL,&signLen, hash, SHA256_DIGEST_LENGTH));
  //std::cout << KCYN << "signing text using EC - len:" << signLen << "-" << SIGN_LEN << KNRM << std::endl;
  //sign.assign(signLen,0);
  //EVP_MD_CTX * md_ctx = EVP_MD_CTX_new();
  //EVP_MD_CTX_set_pkey_ctx(md_ctx, key_ctx);
  assert(1==EVP_PKEY_sign(key_ctx, sign, &signLen, hash, SHA256_DIGEST_LENGTH));

  EVP_PKEY_CTX_free(key_ctx);
  EVP_PKEY_free(key);
}


void old_signTextEC(PID id, std::string text, EC_key priv, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "signing text using EC" << KNRM << std::endl; }
  unsigned int signLen;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  if (!ECDSA_sign (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, &signLen, priv)) {
    std::cout << KCYN << "ECDSA_sign failed(" << signLen << ")" << KNRM << std::endl;
    exit(0);
  }
  //if (DEBUG) { std::cout << KCYN << "signature size: " << signLen << KNRM << std::endl; }
}


bool verifyTextRSA(std::string text, RSA_key pub, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "verifying text using RSA" << KNRM << std::endl; }
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  EVP_PKEY * key = EVP_PKEY_new();
  assert(1==EVP_PKEY_assign_RSA(key, pub));

  EVP_PKEY_CTX * key_ctx = EVP_PKEY_CTX_new(key,NULL);

  assert(1==EVP_PKEY_verify_init(key_ctx));
  assert(1==EVP_PKEY_CTX_set_signature_md(key_ctx, EVP_sha256()));

  //EVP_MD_CTX * md_ctx = EVP_MD_CTX_new();
  //EVP_MD_CTX_set_pkey_ctx(md_ctx, key_ctx);
  const int b=EVP_PKEY_verify(key_ctx, sign, signLen, hash, SHA256_DIGEST_LENGTH);

  EVP_PKEY_CTX_free(key_ctx);
  EVP_PKEY_free(key);

  return b;
}


bool old_verifyTextRSA(std::string text, RSA_key pub, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "verifying text using RSA" << KNRM << std::endl; }
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  bool b = RSA_verify (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, signLen, pub);

  // printHex("VERIFY", sign, signLen);
  // printf("Signature length = %d\n", signLen);

  return b;
}


bool verifyTextEC(std::string text, EC_key pub, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "verifying text using EC" << KNRM << std::endl; }
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

//  bool b = ECDSA_verify (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, signLen, pub);

  EVP_PKEY * key = EVP_PKEY_new();
  assert(1==EVP_PKEY_assign_EC_KEY(key, pub));

  EVP_PKEY_CTX * key_ctx = EVP_PKEY_CTX_new(key,NULL);

  assert(1==EVP_PKEY_verify_init(key_ctx));
  assert(1==EVP_PKEY_CTX_set_signature_md(key_ctx, EVP_sha256()));

  //EVP_MD_CTX * md_ctx = EVP_MD_CTX_new();
  //EVP_MD_CTX_set_pkey_ctx(md_ctx, key_ctx);
  const int b=EVP_PKEY_verify(key_ctx, sign, signLen, hash, SHA256_DIGEST_LENGTH);

  EVP_PKEY_CTX_free(key_ctx);
  EVP_PKEY_free(key);

  return b;
}


bool old_verifyTextEC(std::string text, EC_key pub, unsigned char sign[SIGN_LEN]) {
  //if (DEBUG) { std::cout << KCYN << "verifying text using EC" << KNRM << std::endl; }
  unsigned int signLen = SIGN_LEN;
  unsigned char hash[SHA256_DIGEST_LENGTH];

  if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash)){
    std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
    exit(0);
  }

  //auto t1 = std::chrono::high_resolution_clock::now();

  bool b = ECDSA_verify (NID_sha256, hash, SHA256_DIGEST_LENGTH, sign, signLen, pub);

  // auto t2 = std::chrono::high_resolution_clock::now();

  // unsigned char hash2[SHA256_DIGEST_LENGTH];

  // if (!SHA256 ((const unsigned char *)text.c_str(), text.size(), hash2)){
  //   std::cout << KCYN << "SHA1 failed" << KNRM << std::endl;
  //   exit(0);
  // }

  // auto t3 = std::chrono::high_resolution_clock::now();

  // bool b2 = ECDSA_verify (NID_sha256, hash2, SHA256_DIGEST_LENGTH, sign, signLen, pub);

  // auto t4 = std::chrono::high_resolution_clock::now();

  // double time1 = std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();
  // double time2 = std::chrono::duration_cast<std::chrono::microseconds>(t4 - t3).count();
  // std::cout << KGRN << ":ECDSA-verify-time1=" << time1 << ":time2=" << time2 << ":hash=" << hash << ":sign-len=" << signLen << KNRM << std::endl;

  return b;
}


int main(int argc, char const *argv[]) {
  unsigned int id = 0;
  KeysFun KF;
  std::string text = "foobar";
  auto start = std::chrono::steady_clock::now();
  auto end = std::chrono::steady_clock::now();
  double time = 0.0;
  unsigned char s[SIGN_LEN];
  unsigned int repetition = 1000;
  double count = 0.0;

  // -----------------------------------------
  // new sign EC signatures
  count = 0.0;

  for (int i = 0; i < repetition; i++) {
    EC_key priv1;
    KF.loadPrivateKey(id,&priv1);

    start = std::chrono::steady_clock::now();
    signTextEC(id, text, priv1, s);
    end = std::chrono::steady_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    count += time;
  }
  std::cout << "new-EC-sig-time:" << count / repetition << std::endl;

  // -----------------------------------------
  // new verify EC signatures
  count = 0.0;

  for (int i = 0; i < repetition; i++) {
    EC_key priv1;
    KF.loadPrivateKey(id,&priv1);
    signTextEC(id, text, priv1, s);
    //std::cout << "signature:" << s << std::endl;

    EC_key pub1;
    KF.loadPublicKey(id,&pub1);

    start = std::chrono::steady_clock::now();
    verifyTextEC(text, pub1, s);
    end = std::chrono::steady_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    count += time;
  }
  std::cout << "new-EC-verif-time:" << count / repetition << std::endl;

  // -----------------------------------------
  // new sign RSA signatures
  count = 0.0;

  for (int i = 0; i < repetition; i++) {
    RSA_key priv1 = RSA_new();
    KF.loadPrivateKey(id,&priv1);

    start = std::chrono::steady_clock::now();
    signTextRSA(id, text, priv1, s);
    end = std::chrono::steady_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    count += time;
  }
  std::cout << "new-RSA-sig-time:" << count / repetition << std::endl;

  // -----------------------------------------
  // new verify RSA signatures
  count = 0.0;

  for (int i = 0; i < repetition; i++) {
    RSA_key priv1 = RSA_new();
    KF.loadPrivateKey(id,&priv1);
    signTextRSA(id, text, priv1, s);
    //std::cout << "signature:" << s << std::endl;

    RSA_key pub1 = RSA_new();
    KF.loadPublicKey(id,&pub1);

    start = std::chrono::steady_clock::now();
    verifyTextRSA(text, pub1, s);
    end = std::chrono::steady_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    count += time;
  }
  std::cout << "new-EC-verif-time:" << count / repetition << std::endl;


  // -----------------------------------------
  // old sign EC signatures
  count = 0.0;

  EC_key pub2;
  EC_key priv2;
  KF.loadPublicKey(id,&pub2);
  KF.loadPrivateKey(id,&priv2);

  for (int i = 0; i < repetition; i++) {
    start = std::chrono::steady_clock::now();
    old_signTextEC(id, text, priv2, s);
    end = std::chrono::steady_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    count += time;
  }
  std::cout << "old-EC-sig-time:" << count / repetition << std::endl;

  // -----------------------------------------
  // old verify EC signatures
  count = 0.0;

  for (int i = 0; i < repetition; i++) {
    old_signTextEC(id, text, priv2, s);
    //std::cout << "signature:" << s << std::endl;
    start = std::chrono::steady_clock::now();
    old_verifyTextEC(text, pub2, s);
    end = std::chrono::steady_clock::now();
    time = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    count += time;
  }
  std::cout << "old-EC-verif-time:" << count / repetition << std::endl;

  return 0;
}
