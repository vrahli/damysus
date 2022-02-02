#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>

#include <openssl/rand.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>


#include "config.h"
#include "KeysFun.h"


std::string dir = "somekeys/";


int KeysFun::loadPrivateKey(PID id, NO_key* priv) { return 0; }
int KeysFun::loadPublicKey(PID pid, NO_key* pub) { return 0; }


int KeysFun::loadPrivateKey(PID id, RSA_key* priv) {
    if (DEBUG) std::cout << KYEL << "loading private key" << KNRM << std::endl;
    std::string pr;
#ifdef KK_RSA4096
    pr = dir + "rsa4096_private" + std::to_string(id);
#endif
#ifdef KK_RSA2048
    pr = dir + "rsa2048_private" + std::to_string(id);
#endif
    FILE * fpr = fopen (pr.c_str(), "rb");

    if (fpr == NULL) {
        if (DEBUG) std::cout << KYEL << "Unable to open file " << pr << KNRM << std::endl;
        return 1;
    }
    *priv = PEM_read_RSAPrivateKey (fpr, priv, NULL, NULL);
    fclose(fpr);
    if (DEBUG) std::cout << KYEL << "loaded private key from " << pr << KNRM << std::endl;
    return 0;
}


int KeysFun::loadPublicKey(PID pid, RSA_key* pub) {
    // Loading public key
    if (DEBUG) std::cout << KMAG << "loading public key" << KNRM << std::endl;
    std::string pb;

#ifdef KK_RSA4096
    pb = dir + "rsa4096_public" + std::to_string(pid);
#endif
#ifdef KK_RSA2048
    pb = dir + "rsa2048_public" + std::to_string(pid);
#endif
    FILE * fpb = fopen (pb.c_str(), "rb");

    if (fpb == NULL) {
        if (DEBUG) std::cout << KYEL << "Unable to open file " << pb << KNRM << std::endl;
        return 1;
    }
    if (DEBUG) std::cout << KMAG << "loading key from " << pb << KNRM << std::endl;
    *pub = PEM_read_RSAPublicKey (fpb, pub, NULL, NULL);
    fclose(fpb);
    if (DEBUG) std::cout << KMAG << "loaded public key from " << pb << KNRM << std::endl;
    return 0;
}


int KeysFun::loadPrivateKey(PID id, EVP_key* priv) {
    if (DEBUG) std::cout << KYEL << "loading private key" << KNRM << std::endl;
    std::string pr;
#ifdef KK_EC521
    pr = dir + "ec521_private" + std::to_string(id);
#endif
#ifdef KK_EC256
    pr = dir + "ec256_private" + std::to_string(id);
#endif
    FILE * fpr = fopen (pr.c_str(), "rb");

    if (fpr == NULL) {
        if (DEBUG) std::cout << KYEL << "Unable to open file " << pr << KNRM << std::endl;
        return 1;
    }

    //EVP_PKEY *pkey = EVP_PKEY_new();
    EC_KEY *p = PEM_read_ECPrivateKey(fpr, NULL, NULL, NULL);
    if (!EC_KEY_check_key(p)) { std::cout << KYEL << "invalid key" << KNRM << std::endl; }

    *priv = EVP_PKEY_new();
    if (!EVP_PKEY_assign_EC_KEY(*priv, p)) { std::cout << KYEL << "couldn't assign EC key" << KNRM << std::endl; }

    //*priv = EVP_PKEY_get1_EC_KEY(pkey);
    fclose(fpr);
    if (DEBUG) std::cout << KYEL << "loaded private key from " << pr << KNRM << std::endl;

    // free the pkey
    //EVP_PKEY_free(pkey);

    return 0;
}


int KeysFun::loadPublicKey(PID id, EVP_key* pub) {
    if (DEBUG) std::cout << KYEL << "loading public key" << KNRM << std::endl;
    std::string pb;
#ifdef KK_EC521
    pb = dir + "ec521_public" + std::to_string(id);
#endif
#ifdef KK_EC256
    pb = dir + "ec256_public" + std::to_string(id);
#endif
    FILE * fpb = fopen (pb.c_str(), "rb");

    if (fpb == NULL) {
        if (DEBUG) std::cout << KYEL << "Unable to open file " << pb << KNRM << std::endl;
        return 1;
    }

    //EVP_PKEY *pkey = EVP_PKEY_new();
    EC_KEY *p = PEM_read_EC_PUBKEY(fpb, NULL, NULL, NULL);
    if (!EC_KEY_check_key(p)) { std::cout << KYEL << "invalid key" << KNRM << std::endl; }

    *pub = EVP_PKEY_new();
    if (!EVP_PKEY_assign_EC_KEY(*pub, p)) { std::cout << KYEL << "couldn't assign EC key" << KNRM << std::endl; }

    //*pub = EVP_PKEY_get1_EC_KEY(pkey);
    fclose(fpb);
    if (DEBUG) std::cout << KYEL << "loaded public key from " << pb << KNRM << std::endl;

    // free the pkey
    //EVP_PKEY_free(pkey);

    return 0;
}



int KeysFun::loadPrivateKey(PID id, EC_key* priv) {
    if (DEBUG) std::cout << KYEL << "loading private key" << KNRM << std::endl;
    std::string pr;
#ifdef KK_EC521
    pr = dir + "ec521_private" + std::to_string(id);
#endif
#ifdef KK_EC256
    pr = dir + "ec256_private" + std::to_string(id);
#endif
    FILE * fpr = fopen (pr.c_str(), "rb");

    if (fpr == NULL) {
      if (DEBUG) std::cout << KYEL << "Unable to open file " << pr << KNRM << std::endl;
      return 1;
    }

    *priv = PEM_read_ECPrivateKey(fpr, NULL, NULL, NULL);
    if (!EC_KEY_check_key(*priv)) { std::cout << KYEL << "invalid key" << KNRM << std::endl; }

    fclose(fpr);
    if (DEBUG) std::cout << KYEL << "loaded private key from " << pr << KNRM << std::endl;

    return 0;
}


int KeysFun::loadPublicKey(PID id, EC_key* pub) {
    if (DEBUG) std::cout << KYEL << "loading public key" << KNRM << std::endl;
    std::string pb;
#ifdef KK_EC521
    pb = dir + "ec521_public" + std::to_string(id);
#endif
#ifdef KK_EC256
    pb = dir + "ec256_public" + std::to_string(id);
#endif
    FILE * fpb = fopen (pb.c_str(), "rb");

    if (fpb == NULL) {
        if (DEBUG) std::cout << KYEL << "Unable to open file " << pb << KNRM << std::endl;
        return 1;
    }

    *pub = PEM_read_EC_PUBKEY(fpb, NULL, NULL, NULL);
    if (!EC_KEY_check_key(*pub)) { std::cout << KYEL << "invalid key" << KNRM << std::endl; }

    fclose(fpb);
    if (DEBUG) std::cout << KYEL << "loaded public key from " << pb << KNRM << std::endl;

    return 0;
}



void KeysFun::generateRsa4096Keys(int id) {
  //The pseudo-random number generator must be seeded prior to calling RSA_generate_key_ex function
  unsigned char seed[] = {0x58 ,0x48 ,0x54 ,0x4f ,0x36 ,0x65 ,0x69 ,0x47};
  RAND_seed(seed,8);

  RSA* rsa = RSA_new();//allocate empty key
  unsigned long e = RSA_F4 ; //65537 public exponent
  BIGNUM* bne = BN_new(); // allocate BINNUM structure in heap
  BN_set_word(bne,e); //store that public exponent in big-number object bne

  // generate RSA key with length 4096 , public exponent 65537
  RSA_generate_key_ex(rsa,RSA_NUM_BITS4k,bne,NULL);

  //allocate a memory BIO in heap
  BIO* bio_private = BIO_new(BIO_s_mem());
  BIO* bio_public  = BIO_new(BIO_s_mem());

  //extract private and public key to bio-object respectively
  PEM_write_bio_RSAPrivateKey(bio_private, rsa, NULL, NULL, 0, NULL, NULL);
  PEM_write_bio_RSAPublicKey(bio_public, rsa);

  //BIO_pending function return number of byte read to bio buffer during previous step
  int private_len = BIO_pending(bio_private);
  int public_len  = BIO_pending(bio_public);

  //this two buffer will hold the keys as string
  char* private_key = new char[private_len + 1];
  char* public_key  = new char[public_len + 1];

  //copy extracted keys to string
  BIO_read(bio_private, private_key, private_len);
  BIO_read(bio_public, public_key, public_len);

  //ensure that both keys ends with null terminator
  private_key[private_len]=0;
  public_key[public_len]=0;

  // std::cout << private_key << std::endl << std::endl << std::endl << std::endl;
  // std::cout << public_key  << std::endl << std::endl << std::endl << std::endl;

  std::string pr = dir + "rsa4096_private" + std::to_string(id);
  std::cout << "writing private key to " << pr << std::endl;
  std::ofstream priv;
  priv.open (pr);
  priv << private_key;
  priv.close();

  std::string pu = dir + "rsa4096_public" + std::to_string(id);
  std::cout << "writing public key to " << pu << std::endl;
  std::ofstream pub;
  pub.open (pu);
  pub << public_key;
  pub.close();

  //clean up memory
  RSA_free(rsa);
  BN_free(bne);
  BIO_free(bio_private);
  BIO_free(bio_public);
  delete [] private_key;
  delete [] public_key;
  RAND_cleanup();
}


void KeysFun::generateRsa2048Keys(int id) {
  //The pseudo-random number generator must be seeded prior to calling RSA_generate_key_ex function
  unsigned char seed[] = {0x58 ,0x48 ,0x54 ,0x4f ,0x36 ,0x65 ,0x69 ,0x47};
  RAND_seed(seed,8);

  RSA* rsa = RSA_new();//allocate empty key
  unsigned long e = RSA_F4 ; //65537 public exponent
  BIGNUM* bne = BN_new(); // allocate BINNUM structure in heap
  BN_set_word(bne,e); //store that public exponent in big-number object bne

  // generate RSA key with length 2048 , public exponent 65537
  RSA_generate_key_ex(rsa,RSA_NUM_BITS2k,bne,NULL);

  //allocate a memory BIO in heap
  BIO* bio_private = BIO_new(BIO_s_mem());
  BIO* bio_public  = BIO_new(BIO_s_mem());

  //extract private and public key to bio-object respectively
  PEM_write_bio_RSAPrivateKey(bio_private, rsa, NULL, NULL, 0, NULL, NULL);
  PEM_write_bio_RSAPublicKey(bio_public, rsa);

  //BIO_pending function return number of byte read to bio buffer during previous step
  int private_len = BIO_pending(bio_private);
  int public_len  = BIO_pending(bio_public);

  //this two buffer will hold the keys as string
  char* private_key = new char[private_len + 1];
  char* public_key  = new char[public_len + 1];

  //copy extracted keys to string
  BIO_read(bio_private, private_key, private_len);
  BIO_read(bio_public, public_key, public_len);

  //ensure that both keys ends with null terminator
  private_key[private_len]=0;
  public_key[public_len]=0;

  // std::cout << private_key << std::endl << std::endl << std::endl << std::endl;
  // std::cout << public_key  << std::endl << std::endl << std::endl << std::endl;

  std::string pr = dir + "rsa2048_private" + std::to_string(id);
  std::cout << "writing private key to " << pr << std::endl;
  std::ofstream priv;
  priv.open (pr);
  priv << private_key;
  priv.close();

  std::string pu = dir + "rsa2048_public" + std::to_string(id);
  std::cout << "writing public key to " << pu << std::endl;
  std::ofstream pub;
  pub.open (pu);
  pub << public_key;
  pub.close();

  //clean up memory
  RSA_free(rsa);
  BN_free(bne);
  BIO_free(bio_private);
  BIO_free(bio_public);
  delete [] private_key;
  delete [] public_key;
  RAND_cleanup();
}


// based on http://fm4dd.com/openssl/eckeycreate.htm
void KeysFun::generateEc521Keys(int id) {
  BIO      *outbio = NULL;
  EC_KEY   *myecc  = NULL;
  EVP_PKEY *pkey   = NULL;
  int      eccgrp;

  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();

  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  outbio = BIO_new(BIO_s_mem());

  /* ---------------------------------------------------------- *
   * Create a EC key sructure, setting the group type from NID  *
   * ---------------------------------------------------------- */
  eccgrp = OBJ_txt2nid("secp521r1");
  myecc = EC_KEY_new_by_curve_name(eccgrp);

  /* -------------------------------------------------------- *
   * For cert signing, we use  the OPENSSL_EC_NAMED_CURVE flag*
   * ---------------------------------------------------------*/
  EC_KEY_set_asn1_flag(myecc, OPENSSL_EC_NAMED_CURVE);

  /* -------------------------------------------------------- *
   * Create the public/private EC key pair here               *
   * ---------------------------------------------------------*/
  if (! (EC_KEY_generate_key(myecc)))
    std::cout << KMAG << "Error generating the ECC key" << KNRM << std::endl;

  /* -------------------------------------------------------- *
   * Converting the EC key into a PKEY structure let us       *
   * handle the key just like any other key pair.             *
   * ---------------------------------------------------------*/
  pkey=EVP_PKEY_new();
  if (!EVP_PKEY_assign_EC_KEY(pkey,myecc))
    std::cout << KMAG << "Error assigning ECC key to EVP_PKEY structure" << KNRM << std::endl;

  /* -------------------------------------------------------- *
   * Now we show how to extract EC-specifics from the key     *
   * ---------------------------------------------------------*/
  myecc = EVP_PKEY_get1_EC_KEY(pkey);
  const EC_GROUP *ecgrp = EC_KEY_get0_group(myecc);

  /* ---------------------------------------------------------- *
   * Here we print the key length, and extract the curve type.  *
   * ---------------------------------------------------------- */
  std::cout << KMAG << "ECC Key size: " << EVP_PKEY_bits(pkey) << " bit" << KNRM << std::endl;
  std::cout << KMAG << "ECC Key type: " << OBJ_nid2sn(EC_GROUP_get_curve_name(ecgrp)) << KNRM << std::endl;

  /* ---------------------------------------------------------- *
   * Here we print the private/public key data in PEM format.   *
   * ---------------------------------------------------------- */
  // --- private key
  if(!PEM_write_bio_PrivateKey(outbio, pkey, NULL, NULL, 0, 0, NULL))
    std::cout << KMAG << "Error writing private key data in PEM format" << KNRM << std::endl;
  //BIO_pending function return number of byte read to bio buffer during previous step
  int private_len = BIO_pending(outbio);
  //std::cout << "private length: " << private_len << std::endl;
  char* private_key = new char[private_len + 1];
  BIO_read(outbio, private_key, private_len);
  private_key[private_len]=0;

  // --- public key
  if(!PEM_write_bio_PUBKEY(outbio, pkey))
    std::cout << KMAG << "Error writing public key data in PEM format" << KNRM << std::endl;
  //BIO_pending function return number of byte read to bio buffer during previous step
  int public_len  = BIO_pending(outbio);
  //std::cout << "public length: " << public_len << std::endl;
  char* public_key  = new char[public_len + 1];
  BIO_read(outbio, public_key, public_len);
  public_key[public_len]=0;

  // printing keys to files
  std::string pr = dir + "ec521_private" + std::to_string(id);
  std::cout << KMAG << "writing EC private key to " << pr << KNRM << std::endl;
  std::ofstream priv;
  priv.open (pr);
  priv << private_key;
  priv.close();

  std::string pu = dir + "ec521_public" + std::to_string(id);
  std::cout << KMAG << "writing EC public key to " << pu << KNRM << std::endl;
  std::ofstream pub;
  pub.open (pu);
  pub << public_key;
  pub.close();

  /* ---------------------------------------------------------- *
   * Free up all structures                                     *
   * ---------------------------------------------------------- */
  EVP_PKEY_free(pkey);
  EC_KEY_free(myecc);
  BIO_free_all(outbio);
}


// based on http://fm4dd.com/openssl/eckeycreate.htm
void KeysFun::generateEc256Keys(int id) {
  BIO      *outbio = NULL;
  EC_KEY   *myecc  = NULL;
  EVP_PKEY *pkey   = NULL;
  int      eccgrp;

  /* ---------------------------------------------------------- *
   * These function calls initialize openssl for correct work.  *
   * ---------------------------------------------------------- */
  OpenSSL_add_all_algorithms();
  ERR_load_BIO_strings();
  ERR_load_crypto_strings();

  /* ---------------------------------------------------------- *
   * Create the Input/Output BIO's.                             *
   * ---------------------------------------------------------- */
  outbio = BIO_new(BIO_s_mem());

  /* ---------------------------------------------------------- *
   * Create a EC key sructure, setting the group type from NID  *
   * ---------------------------------------------------------- */
  eccgrp = OBJ_txt2nid("prime256v1");
  myecc = EC_KEY_new_by_curve_name(eccgrp);

  /* -------------------------------------------------------- *
   * For cert signing, we use  the OPENSSL_EC_NAMED_CURVE flag*
   * ---------------------------------------------------------*/
  EC_KEY_set_asn1_flag(myecc, OPENSSL_EC_NAMED_CURVE);

  /* -------------------------------------------------------- *
   * Create the public/private EC key pair here               *
   * ---------------------------------------------------------*/
  if (! (EC_KEY_generate_key(myecc)))
    std::cout << KMAG << "Error generating the ECC key" << KNRM << std::endl;

  /* -------------------------------------------------------- *
   * Converting the EC key into a PKEY structure let us       *
   * handle the key just like any other key pair.             *
   * ---------------------------------------------------------*/
  pkey=EVP_PKEY_new();
  if (!EVP_PKEY_assign_EC_KEY(pkey,myecc))
    std::cout << KMAG << "Error assigning ECC key to EVP_PKEY structure" << KNRM << std::endl;

  /* -------------------------------------------------------- *
   * Now we show how to extract EC-specifics from the key     *
   * ---------------------------------------------------------*/
  myecc = EVP_PKEY_get1_EC_KEY(pkey);
  const EC_GROUP *ecgrp = EC_KEY_get0_group(myecc);

  /* ---------------------------------------------------------- *
   * Here we print the key length, and extract the curve type.  *
   * ---------------------------------------------------------- */
  std::cout << KMAG << "ECC Key size: " << EVP_PKEY_bits(pkey) << " bit" << KNRM << std::endl;
  std::cout << KMAG << "ECC Key type: " << OBJ_nid2sn(EC_GROUP_get_curve_name(ecgrp)) << KNRM << std::endl;

  /* ---------------------------------------------------------- *
   * Here we print the private/public key data in PEM format.   *
   * ---------------------------------------------------------- */
  // --- private key
  if(!PEM_write_bio_PrivateKey(outbio, pkey, NULL, NULL, 0, 0, NULL))
    std::cout << KMAG << "Error writing private key data in PEM format" << KNRM << std::endl;
  //BIO_pending function return number of byte read to bio buffer during previous step
  int private_len = BIO_pending(outbio);
  //std::cout << "private length: " << private_len << std::endl;
  char* private_key = new char[private_len + 1];
  BIO_read(outbio, private_key, private_len);
  private_key[private_len]=0;

  // --- public key
  if(!PEM_write_bio_PUBKEY(outbio, pkey))
    std::cout << KMAG << "Error writing public key data in PEM format" << KNRM << std::endl;
  //BIO_pending function return number of byte read to bio buffer during previous step
  int public_len  = BIO_pending(outbio);
  //std::cout << "public length: " << public_len << std::endl;
  char* public_key  = new char[public_len + 1];
  BIO_read(outbio, public_key, public_len);
  public_key[public_len]=0;

  // printing keys to files
  std::string pr = dir + "ec256_private" + std::to_string(id);
  std::cout << KMAG << "writing EC private key to " << pr << KNRM << std::endl;
  std::ofstream priv;
  priv.open (pr);
  priv << private_key;
  priv.close();

  std::string pu = dir + "ec256_public" + std::to_string(id);
  std::cout << KMAG << "writing EC public key to " << pu << KNRM << std::endl;
  std::ofstream pub;
  pub.open (pu);
  pub << public_key;
  pub.close();

  /* ---------------------------------------------------------- *
   * Free up all structures                                     *
   * ---------------------------------------------------------- */
  EVP_PKEY_free(pkey);
  EC_KEY_free(myecc);
  BIO_free_all(outbio);
}
