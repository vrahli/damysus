#include <iostream>
#include <fstream>
#include <unistd.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <string>
#include <cstring>
#include <thread>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>

#include "Message.h"
#include "RData.h"
#include "Signs.h"
#include "Nodes.h"
#include "KeysFun.h"
#include "Handler.h"



int main(int argc, char const *argv[]) {
  fd_set read_fds;
  fd_set write_fds;
  KeysFun kf;

  // Geting inputs
  if (DEBUG) std::cout << KYEL << "parsing inputs" << KNRM << std::endl;

  unsigned int myid = 0;
  if (argc > 1) { sscanf(argv[1], "%d", &myid); }
  std::cout << KYEL << "[id=" << myid << "]" << KNRM << std::endl;

  unsigned int numFaults = 1;
  if (argc > 2) { sscanf(argv[2], "%d", &numFaults); }
  std::cout << KYEL << "[" << myid << "]#faults=" << numFaults << KNRM << std::endl;

  unsigned int constFactor = 3; // default value: by default, there are 3f+1 nodes
  if (argc > 3) { sscanf(argv[3], "%d", &constFactor); }
  std::cout << KYEL << "[" << myid << "]constFactor=" << constFactor << KNRM << std::endl;

  unsigned int numViews = 10;
  if (argc > 4) { sscanf(argv[4], "%d", &numViews); }
  std::cout << KYEL << "[" << myid << "]#views=" << numViews << KNRM << std::endl;

  double timeout = 5; // timeout in seconds
  if (argc > 5) { sscanf(argv[5], "%lf", &timeout); }
  std::cout << KYEL << "[" << myid << "]timeout=" << timeout << KNRM << std::endl;


  // -- Public key
  KEY priv;
  // Set private key - nothing special to do for EC
#if defined(KK_RSA4096) || defined(KK_RSA2048)
  priv = RSA_new();
#endif

// NOTE: For now, when using the accumulator, all nodes use the same keys, as public keys need to be shared
// between 'trusted' components and 'normal' components, and currently keys are hard coded in trusted components.
// We only do that for ec256, because that's the only one we use really right now.
// We do the same for public keys below.
#if /*(defined(ACCUM) || defined(COMB)) &&*/ defined(KK_EC256)
  BIO *bio = BIO_new(BIO_s_mem());
  int w = BIO_write(bio,priv_key256,sizeof(priv_key256));
  priv = PEM_read_bio_ECPrivateKey(bio, NULL, NULL, NULL);
#else
  if (kf.loadPrivateKey(myid,&priv)) {
    std::cout << KYEL << "loading private key failed" << KNRM << std::endl;
    return 0;
  }
#endif

#ifdef KK_EC256
  if (DEBUG1) { std::cout << KYEL << "checking private key" << KNRM << std::endl; }
  if (!EC_KEY_check_key(priv)) {
    std::cout << KYEL << "invalid key" << KNRM << std::endl;
  }
  if (DEBUG1) { std::cout << KYEL << "checked private key (sign size=" << ECDSA_size(priv) << ")" << KNRM << std::endl; }
#endif


  unsigned int numNodes = (constFactor*numFaults)+1;
  std::string confFile = "config";
  Nodes nodes(confFile,numNodes);


  // -- Public keys
  for (unsigned int i = 0; i < numNodes; i++) {
    //public key
    KEY pub;
    // Set public key - nothing special to do for EC
#if defined(KK_RSA4096) || defined(KK_RSA2048)
    pub = RSA_new();
#endif
#if /*(defined(ACCUM) || defined(COMB)) &&*/ defined(KK_EC256)
    BIO *bio = BIO_new(BIO_s_mem());
    int w = BIO_write(bio,pub_key256,sizeof(pub_key256));
    pub = PEM_read_bio_EC_PUBKEY(bio, NULL, NULL, NULL);
#else
    kf.loadPublicKey(i,&pub);
#endif
    if (DEBUG) std::cout << KMAG << "id: " << i << KNRM << std::endl;
    nodes.setPub(i,pub);
  }


  // Initializing handler
  if (DEBUG) std::cout << KYEL << "initializing handler" << KNRM << std::endl;

  long unsigned int size = std::max({sizeof(MsgTransaction), sizeof(MsgReply), sizeof(MsgStart)});

  #if defined(BASIC_CHEAP) || defined(BASIC_BASELINE)
  size = std::max({size,
                   sizeof(MsgNewView),
                   sizeof(MsgPrepare),
                   sizeof(MsgLdrPrepare),
                   sizeof(MsgPreCommit),
                   sizeof(MsgCommit)});
  #elif defined(BASIC_QUICK) || defined(BASIC_QUICK_DEBUG)
  size = std::max({size,
                   sizeof(MsgNewViewAcc),
                   sizeof(MsgLdrPrepareAcc),
                   sizeof(MsgPrepareAcc),
                   sizeof(MsgPreCommitAcc)});
  #elif defined(BASIC_CHEAP_AND_QUICK)
  size = std::max({size,
                   sizeof(MsgNewViewComb),
                   sizeof(MsgLdrPrepareComb),
                   sizeof(MsgPrepareComb),
                   sizeof(MsgPreCommitComb)});
  #elif defined(BASIC_FREE)
  size = std::max({size,
                   sizeof(MsgNewViewFree),
                   sizeof(MsgLdrPrepareFree),
                   sizeof(MsgBckPrepareFree),
                   sizeof(MsgPrepareFree),
                   sizeof(MsgPreCommitFree)});
  #elif defined(BASIC_ONEP)
  size = std::max({size,
                   sizeof(MsgNewViewOPA),
                   sizeof(MsgNewViewOPB),
                   sizeof(MsgLdrPrepareOPA),
                   sizeof(MsgLdrPrepareOPB),
                   sizeof(MsgBckPrepareOP),
                   sizeof(MsgPreCommitOP)});
  #elif defined(CHAINED_BASELINE)
  size = std::max({size,
                   sizeof(MsgNewViewCh),
                   sizeof(MsgLdrPrepareCh),
                   sizeof(MsgPrepareCh)});
  #elif defined(CHAINED_CHEAP_AND_QUICK) || defined(CHAINED_CHEAP_AND_QUICK_DEBUG)
  size = std::max({size,
                   sizeof(MsgNewViewChComb),
                   sizeof(MsgLdrPrepareChComb),
                   sizeof(MsgPrepareChComb)});
  #endif

  if (DEBUG0) {
    std::cout << KYEL << "[" << myid << "]sizes"
              << ":newview="        << sizeof(MsgNewView)
              << ":prepare="        << sizeof(MsgPrepare)
              << ":ldrprepare="     << sizeof(MsgLdrPrepare)
              << ":precommit="      << sizeof(MsgPreCommit)
              << ":commit="         << sizeof(MsgCommit)
              << ":transaction="    << sizeof(MsgTransaction)
              //<< ":start="          << sizeof(MsgStart)
              << ":newviewacc="     << sizeof(MsgNewViewAcc)
              << ":ldrprepareacc="  << sizeof(MsgLdrPrepareAcc)
              << ":prepareacc="     << sizeof(MsgPrepareAcc)
              << ":precommitacc="   << sizeof(MsgPreCommitAcc)
              << ":newviewcomb="    << sizeof(MsgNewViewComb)
              << ":ldrpreparecomb=" << sizeof(MsgLdrPrepareComb)
              << ":preparecomb="    << sizeof(MsgPrepareComb)
              << ":precommitcomb="  << sizeof(MsgPreCommitComb)
              << ":newviewfree="    << sizeof(MsgNewViewFree)
              << ":ldrpreparefree=" << sizeof(MsgLdrPrepareFree)
              << ":bckpreparefree=" << sizeof(MsgBckPrepareFree)
              << ":preparefree="    << sizeof(MsgPrepareFree)
              << ":precommitfree="  << sizeof(MsgPreCommitFree)
              << KNRM << std::endl;
  }
  if (DEBUG0) std::cout << KYEL << "[" << myid << "]max-msg-size=" << size << KNRM << std::endl;
  PeerNet::Config pconfig;
  // TODO: for some reason 'size' is not quite right
  pconfig.max_msg_size(2*size);
  ClientNet::Config cconfig;
  cconfig.max_msg_size(2*size);
  //config.ping_period(2);
  if (DEBUG1) std::cout << KYEL << "[" << myid << "]starting handler" << KNRM << std::endl;
  Handler handler(kf,myid,timeout,constFactor,numFaults,numViews,nodes,priv,pconfig,cconfig);

  return 0;
};
