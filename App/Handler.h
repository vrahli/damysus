#ifndef HANDLER_H
#define HANDLER_H


#include <map>
#include <set>

#include "Message.h"
#include "Nodes.h"
#include "Log.h"
#include "Stats.h"
#include "Vote.h"
#include "../Enclave/user_types.h"



// ------------------------------------
// SGX related stuff
#if defined(BASIC_CHEAP) || defined(BASIC_QUICK) || defined(BASIC_CHEAP_AND_QUICK) || defined(CHAINED_CHEAP_AND_QUICK)
//
#include "Enclave_u.h"
#include "sgx_urts.h"
#include "sgx_utils/sgx_utils.h"
//
#else // i.e.,  defined(BASIC_BASELINE) || defined(CHAINED_BASELINE) || defined(BASIC_QUICK_DEBUG) || defined(CHAINED_CHEAP_AND_QUICK_DEBUG)
//
#include "TrustedFun.h"
#include "TrustedAccum.h"
#include "TrustedComb.h"
#include "TrustedCh.h"
#include "TrustedChComb.h"
//
#endif
// ------------------------------------


//#define NDEBUG


// Salticidae related stuff
#include <memory>
#include <cstdio>
#include <functional>
#include "salticidae/msg.h"
#include "salticidae/event.h"
#include "salticidae/network.h"
#include "salticidae/stream.h"

using std::placeholders::_1;
using std::placeholders::_2;


using PeerNet     = salticidae::PeerNetwork<uint8_t>;
using Peer        = std::tuple<PID,salticidae::PeerId>;
using Peers       = std::vector<Peer>;
using ClientNet   = salticidae::ClientNetwork<uint8_t>;
using MsgNet      = salticidae::MsgNetwork<uint8_t>;
// the bool is true if the client hasn't stopped yet
// the 1st int is the number of transactions received from the client
// the 2nd int is the number of transactions replied to
using ClientNfo   = std::tuple<bool,unsigned int,unsigned int,ClientNet::conn_t>;
using Clients     = std::map<CID,ClientNfo>;
using rep_queue_t = salticidae::MPSCQueueEventDriven<std::pair<TID,CID>>;
using Time        = std::chrono::time_point<std::chrono::steady_clock>;


class Handler {

 private:
  PID myid;
  double timeout;                // timeout after which nodes start a new view
  unsigned int numFaults;        // number of faults
  unsigned int qsize;            // quorum size
  unsigned int total;            // total number of nodes
  Nodes nodes;                   // collection of the other nodes
  KEY priv;                      // private key
  View view = 0;                 // current view - initially 0
  unsigned int maxViews = 0;     // 0 means no constraints
  KeysFun kf;                    // To access crypto functions
  Stats stats;                   // To collect statistics

  salticidae::EventContext pec; // peer ec
  salticidae::EventContext cec; // request ec
  //salticidae::EventContext rep_ec;
  PeerNet pnet;
  Peers peers;
  Clients clients;
  ClientNet cnet;
  std::thread c_thread; // request thread
  //std::thread rep_thread; // reply thread
  rep_queue_t rep_queue;
  salticidae::BoxObj<salticidae::ThreadCall> req_tcall;
  //salticidae::BoxObj<salticidae::ThreadCall> rep_tcall;
  unsigned int viewsWithoutNewTrans = 0;
  bool started = false;
  bool stopped = false;
  salticidae::TimerEvent timer;
  View timerView; // view at which the timer was started

  std::list<Transaction> transactions; // current waiting to be processed
  std::map<View,Block> blocks; // blocks received in each view
  std::map<View,JBlock> jblocks; // blocks received in each view (Chained baseline)
  std::map<View,CBlock> cblocks; // blocks received in each view (Chained Cheap&Quick)
  Log log; // log of messages

  // Used for the accumulator version
  Cert qcprep;

  //void newview_handler(MsgNewView &&msg, const PeerNet::conn_t &conn);

  // Initializes SGX-related stuff
  int initializeSGX();

  void printClientInfo();

  // returns the total number of nodes
  unsigned int getTotal();

  // returns the leader of view 'v'
  unsigned int getLeaderOf(View v);

  // returns the current leader
  unsigned int getCurrentLeader();

  // true iff 'myid' is the leader of view 'v'
  bool amLeaderOf(View v);

  // ture iff 'myid' is the leader of the current view
  bool amCurrentLeader();

  // used to print debugging info
  std::string nfo();

  bool timeToStop();
  void recordStats();
  void setTimer();

  // To stop clients once all have stopped
  //void checkStopClients();

  Block createNewBlock(Hash hash);

  void replyTransactions(Transaction *transactions);
  void replyHash(Hash hash);

  // send messages
  //void sendData(unsigned int size, char *data, Peers recipients);
  void sendMsgNewView(MsgNewView msg, Peers recipients);
  void sendMsgPrepare(MsgPrepare msg, Peers recipients);
  void sendMsgLdrPrepare(MsgLdrPrepare msg, Peers recipients);
  void sendMsgPreCommit(MsgPreCommit msg, Peers recipients);
  void sendMsgCommit(MsgCommit msg, Peers recipients);

  //void sendMsgReply(MsgReply msg, ClientNet::conn_t recipient);

  void sendMsgNewViewAcc(MsgNewViewAcc msg, Peers recipients);
  void sendMsgLdrPrepareAcc(MsgLdrPrepareAcc msg, Peers recipients);
  void sendMsgPrepareAcc(MsgPrepareAcc msg, Peers recipients);
  void sendMsgPreCommitAcc(MsgPreCommitAcc msg, Peers recipients);

  void sendMsgNewViewComb(MsgNewViewComb msg, Peers recipients);
  void sendMsgLdrPrepareComb(MsgLdrPrepareComb msg, Peers recipients);
  void sendMsgPrepareComb(MsgPrepareComb msg, Peers recipients);
  void sendMsgPreCommitComb(MsgPreCommitComb msg, Peers recipients);

  void sendMsgNewViewCh(MsgNewViewCh msg, Peers recipients);
  void sendMsgPrepareCh(MsgPrepareCh msg, Peers recipients);
  void sendMsgLdrPrepareCh(MsgLdrPrepareCh msg, Peers recipients);

  void sendMsgNewViewChComb(MsgNewViewChComb msg, Peers recipients);
  void sendMsgPrepareChComb(MsgPrepareChComb msg, Peers recipients);
  void sendMsgLdrPrepareChComb(MsgLdrPrepareChComb msg, Peers recipients);

  // for leaders to start the phase where nodes will log prepare certificates
  void initiatePrepare(RData rdata);
  // for leaders to start the phase where nodes will log lock certificates
  void initiatePrecommit(RData rdata);
  // for leaders to start the phase where nodes execute
  void initiateCommit(RData rdata);

  //bool verifyPrepare(Message<Proposal> msg);
  bool verifyTransaction(MsgTransaction msg);
  //bool verifyStart(MsgStart msg);

  bool verifyJust(Just just);

  bool verifyLdrPrepareComb(MsgLdrPrepareComb msg);
  bool verifyPreCommitCombCert(MsgPreCommitComb msg);

  Accum newviews2acc(std::set<MsgNewViewAcc> newviews);

  // To start the code
  void getStarted();

  void prepare();

  void respondToProposal(Just justNv, Block b);
  void respondToPrepareJust(Just justPrep);
  void respondToPreCommitJust(Just justPc);

  Peers remove_from_peers(PID id);
  Peers keep_from_peers(PID id);

  void startNewViewOnTimeout();


  // ------------------------------------------------------------
  // Baseline and Cheap
  // ------

  void executeRData(RData rdata);
  void handleEarlierMessages();
  void startNewView();

  // Wrappers around the TEE functions
  Just callTEEsign();
  Just callTEEstore(Just j);
  Just callTEEprepare(Hash h, Just j);
  bool callTEEverify(Just j);

  void handleNewview(MsgNewView msg);
  void handlePrepare(MsgPrepare msg);
  void handleLdrPrepare(MsgLdrPrepare msg);
  void handlePrecommit(MsgPreCommit msg);
  void handleCommit(MsgCommit msg);
  void handleTransaction(MsgTransaction msg);
  //void handleStart(MsgStart msg);

  void handle_newview(MsgNewView msg, const PeerNet::conn_t &conn);
  void handle_prepare(MsgPrepare msg, const PeerNet::conn_t &conn);
  void handle_ldrprepare(MsgLdrPrepare msg, const PeerNet::conn_t &conn);
  void handle_precommit(MsgPreCommit msg, const PeerNet::conn_t &conn);
  void handle_commit(MsgCommit msg, const PeerNet::conn_t &conn);
  void handle_transaction(MsgTransaction msg, const ClientNet::conn_t &conn);
  void handle_start(MsgStart msg, const ClientNet::conn_t &conn);
  //void handle_stop(MsgStop msg, const ClientNet::conn_t &conn);



  // ------------------------------------------------------------
  // Quick
  // ------

  void executeCData(CData<Hash,Void> cdata);
  void handleEarlierMessagesAcc();
  void startNewViewAcc();

  // For leaders to start preparing
  void prepareAcc();
  // For leaders to start pre-committing
  void preCommitAcc(CData<Hash,Void> data);
  // For leaders to start deciding
  void decideAcc(CData<Hash,Void> data);

  // For backups to respond to correct MsgLdrPrepareAcc messages received from leaders
  void respondToLdrPrepareAcc(Block block);
  // For backups to respond to MsgPrepareAcc messages receveid from leaders
  void respondToPrepareAcc(MsgPrepareAcc msg);
  // For backups to respond to MsgPreCommitAcc messages receveid from leaders
  void respondToPreCommitAcc(MsgPreCommitAcc msg);

  Accum callTEEaccum(Vote<Void,Cert> votes[MAX_NUM_SIGNATURES]);
  Accum callTEEaccumSp(uvote_t vote);

  bool verifyPrepareAccCert(MsgPrepareAcc msg);
  bool verifyLdrPrepareAcc(MsgLdrPrepareAcc msg);
  bool verifyAcc(Accum acc);
  bool verifyPreCommitAccCert(MsgPreCommitAcc msg);

  // To create MsgPrepareAcc messages in the prepare phase
  MsgPrepareAcc createMsgPrepareAcc(Block block);
  // To create MsgPreCommitAcc messages in the pre-commit phase
  MsgPreCommitAcc createMsgPreCommitAcc(View view, Hash hash);
  // To create MsgNewViewAcc messages at the beginning of a new view
  MsgNewViewAcc createMsgNewViewAcc();

  void handleNewviewAcc(MsgNewViewAcc msg);
  void handlePrepareAcc(MsgPrepareAcc msg);
  void handleLdrPrepareAcc(MsgLdrPrepareAcc msg);
  void handlePreCommitAcc(MsgPreCommitAcc msg);

  void handle_newviewacc(MsgNewViewAcc msg, const PeerNet::conn_t &conn);
  void handle_prepareacc(MsgPrepareAcc msg, const PeerNet::conn_t &conn);
  void handle_ldrprepareacc(MsgLdrPrepareAcc msg, const PeerNet::conn_t &conn);
  void handle_precommitacc(MsgPreCommitAcc msg, const PeerNet::conn_t &conn);



  // ------------------------------------------------------------
  // Cheap&Quick
  // ------

  void executeComb(RData rdata);
  void handleEarlierMessagesComb();
  void startNewViewComb();

  // For leaders to start preparing
  void prepareComb();
  // For leaders to start pre-committing
  void preCommitComb(RData data);
  // For leaders to start deciding
  void decideComb(RData data);

  // For backups to respond to correct MsgLdrPrepareComb messages received from leaders
  void respondToLdrPrepareComb(Block block, Accum acc);
  // For backups to respond to MsgPrepareComb messages receveid from leaders
  void respondToPrepareComb(MsgPrepareComb msg);
  // For backups to respond to MsgPreCommitComb messages receveid from leaders
  void respondToPreCommitComb(MsgPreCommitComb msg);

  Accum newviews2accComb(std::set<MsgNewViewComb> newviews);

  Accum callTEEaccumComb(Just justs[MAX_NUM_SIGNATURES]);
  Accum callTEEaccumCombSp(just_t just);
  Just callTEEsignComb();
  Just callTEEprepareComb(Hash h, Accum acc);
  Just callTEEstoreComb(Just j);

  void handleNewviewComb(MsgNewViewComb msg);
  void handlePrepareComb(MsgPrepareComb msg);
  void handleLdrPrepareComb(MsgLdrPrepareComb msg);
  void handlePreCommitComb(MsgPreCommitComb msg);

  void handle_newviewcomb(MsgNewViewComb msg, const PeerNet::conn_t &conn);
  void handle_preparecomb(MsgPrepareComb msg, const PeerNet::conn_t &conn);
  void handle_ldrpreparecomb(MsgLdrPrepareComb msg, const PeerNet::conn_t &conn);
  void handle_precommitcomb(MsgPreCommitComb msg, const PeerNet::conn_t &conn);



  // ------------------------------------------------------------
  // Baseline Chained
  // ------

  Just justNV;

  void startNewViewCh();

  Just callTEEsignCh();
  Just callTEEprepareCh(JBlock block, JBlock block0, JBlock block1);

  JBlock createNewBlockCh();

  Just ldrPrepareCh2just(MsgLdrPrepareCh msg);

  void tryExecuteCh(JBlock block, JBlock block0, JBlock block1);
  void voteCh(JBlock block);
  void prepareCh();
  void checkNewJustCh(RData data);
  void handleEarlierMessagesCh();

  void handleNewviewCh(MsgNewViewCh msg);
  void handlePrepareCh(MsgPrepareCh msg);
  void handleLdrPrepareCh(MsgLdrPrepareCh msg);

  void handle_newview_ch(MsgNewViewCh msg, const PeerNet::conn_t &conn);
  void handle_prepare_ch(MsgPrepareCh msg, const PeerNet::conn_t &conn);
  void handle_ldrprepare_ch(MsgLdrPrepareCh msg, const PeerNet::conn_t &conn);



  // ------------------------------------------------------------
  // Chained Cheap&Quick
  // ------

  CA caprep;

  void startNewViewChComb();

  Just callTEEsignChComb();
  Just callTEEprepareChComb(CBlock block, Hash hash);
  Accum callTEEaccumChComb(Just justs[MAX_NUM_SIGNATURES]);
  Accum callTEEaccumChCombSp(just_t just);

  CBlock createNewBlockChComb();

  Just ldrPrepareChComb2just(MsgLdrPrepareChComb msg);

  void tryExecuteChComb(CBlock block, CBlock block0);
  void voteChComb(CBlock block);
  void prepareChComb();
  void checkNewJustChComb(RData data);
  void handleEarlierMessagesChComb();
  Accum newviews2accChComb(std::set<MsgNewViewChComb> newviews);

  void handleNewviewChComb(MsgNewViewChComb msg);
  void handlePrepareChComb(MsgPrepareChComb msg);
  void handleLdrPrepareChComb(MsgLdrPrepareChComb msg);

  void handle_newview_ch_comb(MsgNewViewChComb msg, const PeerNet::conn_t &conn);
  void handle_prepare_ch_comb(MsgPrepareChComb msg, const PeerNet::conn_t &conn);
  void handle_ldrprepare_ch_comb(MsgLdrPrepareChComb msg, const PeerNet::conn_t &conn);

 public:
  Handler(KeysFun kf, PID id, unsigned long int timeout, unsigned int constFactor, unsigned int numFaults, unsigned int maxViews, Nodes nodes, KEY priv, PeerNet::Config pconf, ClientNet::Config cconf);
};


#endif
