#ifndef KEYSFUN_H
#define KEYSFUN_H

#include <string>

#include "config.h"
#include "key.h"



/*void newKEY(KEY key);*/

class KeysFun {

    private:
    public:
      int loadPrivateKey(PID id, NO_key*  priv);
      int loadPrivateKey(PID id, RSA_key* priv);
      int loadPrivateKey(PID id, EC_key*  priv);
      int loadPrivateKey(PID id, EVP_key* priv);

      int loadPublicKey(PID id, NO_key*  pub);
      int loadPublicKey(PID id, RSA_key* pub);
      int loadPublicKey(PID id, EC_key*  pub);
      int loadPublicKey(PID id, EVP_key* pub);

      void generateRsa4096Keys(int id);
      void generateRsa2048Keys(int id);
      void generateEc521Keys(int id);
      void generateEc256Keys(int id);
};

#endif
