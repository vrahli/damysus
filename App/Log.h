#ifndef LOG_H
#define LOG_H

#include <set>
#include <map>


#include "Just.h"
#include "Proposal.h"
#include "CData.h"
#include "Void.h"
#include "Cert.h"
#include "Message.h"


class Log {
 private:
  std::map<View,std::set<MsgNewView>> newviews;
  std::map<View,std::set<MsgPrepare>> prepares;
  std::map<View,std::set<MsgPreCommit>> precommits;
  std::map<View,std::set<MsgCommit>> commits;
  std::map<View,std::set<MsgLdrPrepare>> proposals;

  std::map<View,std::set<MsgNewViewAcc>> newviewsAcc;
  std::map<View,std::set<MsgPrepareAcc>> preparesAcc;
  std::map<View,std::set<MsgPreCommitAcc>> precommitsAcc;
  std::map<View,std::set<MsgLdrPrepareAcc>> ldrpreparesAcc;

  std::map<View,std::set<MsgNewViewComb>> newviewsComb;
  std::map<View,std::set<MsgPrepareComb>> preparesComb;
  std::map<View,std::set<MsgPreCommitComb>> precommitsComb;
  std::map<View,std::set<MsgLdrPrepareComb>> ldrpreparesComb;

  std::map<View,std::set<MsgNewViewFree>> newviewsFree;
  std::map<View,std::tuple<PJust>> preparesFree;
  std::map<View,std::set<MsgPreCommitFree>> precommitsFree;
  std::map<View,std::tuple<HAccum>> ldrpreparesFree;

  std::map<View,std::set<OPprepare>> newviewsOPa;
  std::map<View,std::set<OPnvblock>> newviewsOPb;
  std::map<View,std::set<OPstore>> storesOP;
  std::map<View,std::tuple<LdrPrepareOP>> ldrpreparesOP;
  std::map<View,std::set<OPprepare>> preparesOP;
  std::map<View,std::set<OPvote>> votesOP;
/*
  std::map<View,std::tuple<MsgPrepareFree>> preparesOP;
*/

  std::map<View,std::set<MsgNewViewCh>> newviewsCh;
  std::map<View,std::set<MsgPrepareCh>> preparesCh;
  std::map<View,std::set<MsgLdrPrepareCh>> ldrpreparesCh;

  std::map<View,std::set<MsgNewViewChComb>> newviewsChComb;
  std::map<View,std::set<MsgPrepareChComb>> preparesChComb;
  std::map<View,std::set<MsgLdrPrepareChComb>> ldrpreparesChComb;

 public:
  Log();

  // those return the number of signatures (0 if the msg is not from a not-yet-heard-from node)
  unsigned int storeNv(MsgNewView msg);
  unsigned int storePrep(MsgPrepare msg);
  unsigned int storePc(MsgPreCommit msg);
  unsigned int storeCom(MsgCommit msg);
  unsigned int storeProp(MsgLdrPrepare msg);

  unsigned int storeNvAcc(MsgNewViewAcc msg);
  unsigned int storePrepAcc(MsgPrepareAcc msg);
  unsigned int storePcAcc(MsgPreCommitAcc msg);
  unsigned int storeLdrPrepAcc(MsgLdrPrepareAcc msg);

  unsigned int storeNvComb(MsgNewViewComb msg);
  unsigned int storePrepComb(MsgPrepareComb msg);
  unsigned int storePcComb(MsgPreCommitComb msg);
  unsigned int storeLdrPrepComb(MsgLdrPrepareComb msg);

  unsigned int storeNvFree(MsgNewViewFree msg);
  unsigned int storePrepFree(PJust msg);
  unsigned int storeBckPrepFree(MsgBckPrepareFree msg);
  unsigned int storePcFree(MsgPreCommitFree msg);
  unsigned int storeLdrPrepFree(HAccum msg);

  unsigned int storeNvOp(MsgNewViewOPA msg);
  unsigned int storeNvOp(MsgNewViewOPB msg);
  std::set<OPnvblock> getNvOps(View view, unsigned int n);
  unsigned int storeStoreOp(OPstore store);
  OPprepare getOPstores(View view, unsigned int n);
  unsigned int storeLdrPrepOp(LdrPrepareOP msg);
  LdrPrepareOP getLdrPrepareOp(View view);
  unsigned int storePrepareOp(OPprepare prep);
  OPprepare getOPprepare(View view);
  unsigned int storeVoteOp(OPvote vote);
  OPvote getOPvote(View view, unsigned int n);

  unsigned int storeNvCh(MsgNewViewCh msg);
  unsigned int storePrepCh(MsgPrepareCh msg);
  unsigned int storeLdrPrepCh(MsgLdrPrepareCh msg);

  unsigned int storeNvChComb(MsgNewViewChComb msg);
  unsigned int storePrepChComb(MsgPrepareChComb msg);
  unsigned int storeLdrPrepChComb(MsgLdrPrepareChComb msg);

  // finds the justification of the highest message in the 'newviews' log for view 'view'
  Just findHighestNv(View view);
  Just firstPrepare(View view);
  Just firstPrecommit(View view);
  Just firstCommit(View view);
  MsgLdrPrepare firstProposal(View view);

  // collects the signatures of the messages in the 'newviews' log for view 'view', upto 'n' signatures
  Signs getNewView(View view, unsigned int n);
  // collects the signatures of the messages in the 'proposals' log for view 'view', upto 'n' signatures
  Signs getPrepare(View view, unsigned int n);
  // collects the signatures of the messages in the 'precommits' log for view 'view', upto 'n' signatures
  Signs getPrecommit(View view, unsigned int n);
  // collects the signatures of the messages in the 'commits' log for view 'view', upto 'n' signatures
  Signs getCommit(View view, unsigned int n);

  std::set<MsgNewViewAcc> getNewViewAcc(View view, unsigned int n);
  Signs getPrepareAcc(View view, unsigned int n);
  Signs getPrecommitAcc(View view, unsigned int n);

  MsgLdrPrepareAcc firstLdrPrepareAcc(View view);
  MsgPrepareAcc firstPrepareAcc(View view);
  MsgPreCommitAcc firstPrecommitAcc(View view);

  std::set<MsgNewViewComb> getNewViewComb(View view, unsigned int n);
  Signs getPrepareComb(View view, unsigned int n);
  Signs getPrecommitComb(View view, unsigned int n);

  MsgLdrPrepareComb firstLdrPrepareComb(View view);
  MsgPrepareComb firstPrepareComb(View view);
  MsgPreCommitComb firstPrecommitComb(View view);

  std::set<MsgNewViewFree> getNewViewFree(View view, unsigned int n);
  PJust getPrepareFree(View view);
  HAccum getLdrPrepareFree(View view);
  MsgPreCommitFree firstPrecommitFree(View view);
  Auths getPrecommitFree(View view, unsigned int n);

  std::set<MsgNewViewCh> getNewViewCh(View view, unsigned int n);
  Signs getPrepareCh(View view, unsigned int n);

  MsgLdrPrepareCh firstLdrPrepareCh(View view);
  MsgPrepareCh firstPrepareCh(View view);

  Just findHighestNvCh(View view);

  std::set<MsgNewViewChComb> getNewViewChComb(View view, unsigned int n);
  Signs getPrepareChComb(View view, unsigned int n);

  MsgLdrPrepareChComb firstLdrPrepareChComb(View view);
  MsgPrepareChComb firstPrepareChComb(View view);

  Just findHighestNvChComb(View view);

  // generates a string to pretty print logs
  std::string prettyPrint();
};


#endif
