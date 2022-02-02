#ifndef USER_TYPES_H
#define USER_TYPES_H


#include <stdbool.h>

// Interface to the Enclave

#include <openssl/sha.h>
#include "../App/types.h"
#include "../App/config.h"
#include "../App/key.h"


typedef struct _pids_t
{
  unsigned int num_nodes;
  PID pids[MAX_NUM_NODES];
} pids_t;

typedef struct _hash_t
{
  unsigned char hash[SHA256_DIGEST_LENGTH];
  bool set;
} hash_t;

typedef struct _rdata_t
{
  hash_t proph;
  View propv;
  hash_t justh;
  View justv;
  Phase1 phase;
} rdata_t;

typedef struct _sign_t
{
  bool set;
  PID signer;
  unsigned char sign[SIGN_LEN];
} sign_t;

typedef struct _signs_t
{
  unsigned int size;
  sign_t signs[MAX_NUM_SIGNATURES];
} signs_t;

typedef struct _just_t
{
  bool set;
  rdata_t rdata;
  signs_t signs;
} just_t;

typedef struct _onejust_t
{
  bool set;
  rdata_t rdata;
  sign_t sign;
} onejust_t;

typedef struct _onejusts_t
{
  onejust_t justs[MAX_NUM_SIGNATURES];
} onejusts_t;

typedef struct _cert_t
{
  View view;
  hash_t hash;
  signs_t signs;
} cert_t;

typedef struct _cdata_t
{
  Phase1 phase;
  View view;
  cert_t cert;
} cdata_t;

typedef struct _vote_t
{
  cdata_t cdata;
  sign_t sign;
} vote_t;

typedef struct _votes_t
{
  vote_t votes[MAX_NUM_SIGNATURES];
} votes_t;

typedef struct _accum_t
{
  bool set;
  View view;
  View prepv;
  hash_t hash;
  unsigned int size;
  sign_t sign;
} accum_t;

typedef struct _trans_t
{
  CID clientid;
  TID transid;
  unsigned char data[PAYLOAD_SIZE];
} trans_t;

typedef struct _jblock_t
{
  bool set;
  bool executed;
  View view;
  just_t just;
  unsigned int size;
  trans_t trans[MAX_NUM_TRANSACTIONS];
} jblock_t;

typedef struct _ca_t
{
  CAtag tag;
  union{ cert_t cert; accum_t accum; };
} ca_t;

typedef struct _cblock_t
{
  bool set;
  bool executed;
  View view;
  ca_t cert;
  unsigned int size;
  trans_t trans[MAX_NUM_TRANSACTIONS];
} cblock_t;


// "uniform" vote
typedef struct _uvote_t
{
  cdata_t cdata;
  signs_t signs;
} uvote_t;


#endif
