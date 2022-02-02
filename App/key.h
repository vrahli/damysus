#ifndef KEY_H
#define KEY_H


#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/ec.h>

#include "config.h"
#include "types.h"


// Switch between different schemes:
// - KK_NO       --- no signatures
// - KK_RSA4096  --- RSA_4096 signatures
// - KK_RSA2048  --- RSA_2048 signatures
// - KK_EC521    --- elliptic curve signatures
// - KK_EC256    --- elliptic curve signatures
//#define KK_NO
//#define KK_RSA2048
//#define KK_RSA4096
#define KK_EC256
//#define KK_EC521


#define NO_key  unit*
#define RSA_key RSA*
#define EC_key  EC_KEY*
#define EVP_key EVP_PKEY*


#ifdef KK_NO
  #define SIGN_LEN 0   // NO
  typedef NO_key KEY;
#endif
#ifdef KK_RSA4096
  #define SIGN_LEN 512 // RSA_4096
  typedef RSA_key KEY;
#endif
#ifdef KK_RSA2048
  #define SIGN_LEN 256 // RSA_2048
  typedef RSA_key KEY;
#endif
#ifdef KK_EC521
  #define SIGN_LEN 139 // EC_521
  typedef EC_key KEY;
#endif
#ifdef KK_EC256
  #define SIGN_LEN 72 // EC_256
  typedef EC_key KEY;
#endif



const char priv_key256[] = {
  "-----BEGIN PRIVATE KEY-----\n"
  "MIGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgnI0T6AoPs+ufh54e\n"
  "3tr6ywY7KkMBZhBs69NvMpvtXeehRANCAAS+G04ABpuwCvaS0v5fi9vuNOEitPon\n"
  "4nIDK/IJOsGXv85Jw5wayZI19lSB6ox05rLB+CxmEXrDyiOhX8Sz7c0L\n"
  "-----END PRIVATE KEY-----"
/*  "-----BEGIN PRIVATE KEY-----\n"
  "IGHAgEAMBMGByqGSM49AgEGCCqGSM49AwEHBG0wawIBAQQgWoH9Crif3RmM9vK8\n"
  "g3uftiBG3fk/6CVZIjfmaAhH+ChRANCAASLnxAnxhPA+XIJEdJCVB/PMr9RIVpL\n"
  "zd8iNpQTmlGPtrgUgz85P3I5GNsLUejkPqzgQL/yR/LEZWMjy5DPvPN\n"
  "----END PRIVATE KEY-----"*/
};

const char pub_key256[] = {
  "-----BEGIN PUBLIC KEY-----\n"
  "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEvhtOAAabsAr2ktL+X4vb7jThIrT6\n"
  "J+JyAyvyCTrBl7/OScOcGsmSNfZUgeqMdOaywfgsZhF6w8ojoV/Es+3NCw==\n"
  "-----END PUBLIC KEY-----"
/*  "-----BEGIN PUBLIC KEY-----\n"
  "MFkwEwYHKoZIzj0CAQYIKoZIzj0DAQcDQgAEi58QJ8YTwPlyCRHSQlQfzzK/USFa\n"
  "S683fIjaUE5pRj7a4FIM/OT9yORjbC1Ho5D6s4EC/8kfyxGVjI8uQz7zzQ==\n"
  "-----END PUBLIC KEY-----"*/
};

const char priv_key521[] = {
  "-----BEGIN PRIVATE KEY-----\n"
  "MIHuAgEAMBAGByqGSM49AgEGBSuBBAAjBIHWMIHTAgEBBEIAKOMTchhA8v5akxbd\n"
  "1iJXADZfNSQTlC1LYz3c5uJFw8C5/ZYypGkv8iS9QyBY08cQelGeUDMRlqK8oWBD\n"
  "1LgqUBuhgYkDgYYABAH5LqtEK9ycha2uSannCVxtXPTe+0QtH1qc/eaRE25L6aSy\n"
  "p2ouPAuQlbXRNCmN0p9kAOvCZWRYrpVWAqsPyy9kFQAcopQUkZY7AK16FIesa1Qh\n"
  "wWKNsE/Z8ggJBDMI3RyuW44ubVPSYcrBrAjdwoCldzQwpH2koB6e+Q9EHRZBrDN7\n"
  "WQ==\n"
  "-----END PRIVATE KEY-----"
};

const char pub_key521[] = {
  "-----BEGIN PUBLIC KEY-----\n"
  "MIGbMBAGByqGSM49AgEGBSuBBAAjA4GGAAQB+S6rRCvcnIWtrkmp5wlcbVz03vtE\n"
  "LR9anP3mkRNuS+mksqdqLjwLkJW10TQpjdKfZADrwmVkWK6VVgKrD8svZBUAHKKU\n"
  "FJGWOwCtehSHrGtUIcFijbBP2fIICQQzCN0crluOLm1T0mHKwawI3cKApXc0MKR9\n"
  "pKAenvkPRB0WQawze1k=\n"
  "-----END PUBLIC KEY-----"
};


#endif
