#ifndef ENCLAVE_SHARE_H
#define ENCLAVE_SHARE_H

#include "Enclave_t.h"

#include <openssl/pem.h>
#include <openssl/err.h>

#include <string>
#include <map>

#include "../App/config.h"
#include "../App/types.h"
#include "../App/key.h"


std::string nfo();
sgx_status_t initialize_variables(PID *me, pids_t *others, unsigned int *q);

sign_t signString(std::string text);
bool verifyText(signs_t signs, std::string text);
bool verifyJust(just_t *just);
bool verifyAccum(accum_t *acc);

unsigned int getQsize();

std::string hash2string(hash_t hash);
std::string ca2string(ca_t ca);
std::string rdata2string(rdata_t rdata);
std::string cdata2string(cdata_t cdata);
std::string cblock2string(cblock_t *block);
std::string signs2string(signs_t signs);
std::string sign2string(sign_t sign);
bool eqHashes(hash_t h1, hash_t h2);
hash_t newHash();
hash_t noHash();

bool WF(just_t just);
View just2cview(just_t just);
hash_t just2chash(just_t just);
hash_t hashJBlock(jblock_t *block);
hash_t hashCBlock(cblock_t *block);
//sign_t newSign();
//cert_t initCert();
//cblock_t initCBlock();
//hash_t hashInitCBlock();
cblock_t new_cblock_t();

sign_t getN(signs_t signs);

hash_t ca2hash(ca_t ca);
View ca2view(ca_t ca);
View ca2cview(ca_t ca);
signs_t ca2signs(ca_t ca);
bool verifyCA(ca_t ca);

#endif
