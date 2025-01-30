#include "Log.h"

Log::Log() {}

// checks that there are signatures from all the signers
bool msgNewViewFrom(std::set<MsgNewView> msgs, std::set<PID> signers) {
  for (std::set<MsgNewView>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgNewView msg = (MsgNewView)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

bool msgPrepareFrom(std::set<MsgPrepare> msgs, std::set<PID> signers) {
  for (std::set<MsgPrepare>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPrepare msg = (MsgPrepare)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

bool msgPreCommitFrom(std::set<MsgPreCommit> msgs, std::set<PID> signers) {
  for (std::set<MsgPreCommit>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPreCommit msg = (MsgPreCommit)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

bool msgCommitFrom(std::set<MsgCommit> msgs, std::set<PID> signers) {
  for (std::set<MsgCommit>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgCommit msg = (MsgCommit)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

// checks that there are signatures from all the signers
bool msgLdrPrepareFrom(std::set<MsgLdrPrepare> msgs, std::set<PID> signers) {
  for (std::set<MsgLdrPrepare>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgLdrPrepare msg = (MsgLdrPrepare)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

bool msgNewViewAccFrom(std::set<MsgNewViewAcc> msgs, PID signer) {
  for (std::set<MsgNewViewAcc>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgNewViewAcc msg = (MsgNewViewAcc)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgPrepareAccFrom(std::set<MsgPrepareAcc> msgs, std::set<PID> signers) {
  for (std::set<MsgPrepareAcc>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPrepareAcc msg = (MsgPrepareAcc)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

bool msgLdrPrepareAccFrom(std::set<MsgLdrPrepareAcc> msgs, PID signer) {
  for (std::set<MsgLdrPrepareAcc>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgLdrPrepareAcc msg = (MsgLdrPrepareAcc)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgPreCommitAccFrom(std::set<MsgPreCommitAcc> msgs, std::set<PID> signers) {
  for (std::set<MsgPreCommitAcc>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPreCommitAcc msg = (MsgPreCommitAcc)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}


bool msgNewViewCombFrom(std::set<MsgNewViewComb> msgs, PID signer) {
  for (std::set<MsgNewViewComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgNewViewComb msg = (MsgNewViewComb)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgPrepareCombFrom(std::set<MsgPrepareComb> msgs, std::set<PID> signers) {
  for (std::set<MsgPrepareComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPrepareComb msg = (MsgPrepareComb)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

bool msgLdrPrepareCombFrom(std::set<MsgLdrPrepareComb> msgs, PID signer) {
  for (std::set<MsgLdrPrepareComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgLdrPrepareComb msg = (MsgLdrPrepareComb)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgPreCommitCombFrom(std::set<MsgPreCommitComb> msgs, std::set<PID> signers) {
  for (std::set<MsgPreCommitComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPreCommitComb msg = (MsgPreCommitComb)*it;
    std::set<PID> k = msg.signs.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

bool msgNewViewFreeFrom(std::set<MsgNewViewFree> msgs, PID signer) {
  for (std::set<MsgNewViewFree>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgNewViewFree msg = (MsgNewViewFree)*it;
    PID k = msg.auth.getId();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgPrepareFreeFrom(PJust msg, PID signer) {
  std::set<PID> k = msg.getAuths().getSigners();
  if (signer == msg.getAuth().getId()) { return true; }
  for (int i = 0; i < msg.getAuths().getSize(); i++) {
    if (signer == msg.getAuths().get(i).getId()) { return true; }
  }
  return false;
}

bool msgPreCommitFreeFrom(std::set<MsgPreCommitFree> msgs, std::set<PID> signers) {
  for (std::set<MsgPreCommitFree>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPreCommitFree msg = (MsgPreCommitFree)*it;
    std::set<PID> k = msg.auths.getSigners();
    for (std::set<PID>::iterator it2=k.begin(); it2!=k.end(); ++it2) {
      signers.erase((PID)*it2);
      if (signers.empty()) { return true; }
    }
  }
  return false;
}

unsigned int Log::storeNv(MsgNewView msg) {
  RData rdata = msg.rdata;
  View v = rdata.getPropv();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgNewView>>::iterator it1 = this->newviews.find(v);
  if (it1 != this->newviews.end()) { // there is already an entry for this view
    std::set<MsgNewView> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgNewViewFrom(msgs,signers)) {
      msgs.insert(msg);
      this->newviews[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->newviews[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storePrep(MsgPrepare msg) {
  RData rdata = msg.rdata;
  View v = rdata.getPropv();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgPrepare>>::iterator it1 = this->prepares.find(v);
  if (it1 != this->prepares.end()) { // there is already an entry for this view
    std::set<MsgPrepare> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgPrepareFrom(msgs,signers)) {
      msgs.insert(msg);
      this->prepares[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->prepares[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storePc(MsgPreCommit msg) {
  RData rdata = msg.rdata;
  View v = rdata.getPropv();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgPreCommit>>::iterator it1 = this->precommits.find(v);
  if (it1 != this->precommits.end()) { // there is already an entry for this view
    std::set<MsgPreCommit> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a pre-commits message for this view
    if (!msgPreCommitFrom(msgs,signers)) {
      msgs.insert(msg);
      this->precommits[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->precommits[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storeCom(MsgCommit msg) {
  RData rdata = msg.rdata;
  View v = rdata.getPropv();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgCommit>>::iterator it1 = this->commits.find(v);
  if (it1 != this->commits.end()) { // there is already an entry for this view
    std::set<MsgCommit> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a commits message for this view
    if (!msgCommitFrom(msgs,signers)) {
      msgs.insert(msg);
      this->commits[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->commits[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}

unsigned int Log::storeProp(MsgLdrPrepare msg) {
  Proposal prop = msg.prop;
  View v = prop.getJust().getRData().getPropv();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgLdrPrepare>>::iterator it1 = this->proposals.find(v);
  if (it1 != this->proposals.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepare> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a proposal message for this view
    if (!msgLdrPrepareFrom(msgs,signers)) {
      msgs.insert(msg);
      this->proposals[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->proposals[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storeNvAcc(MsgNewViewAcc msg) {
  CData<Void,Cert> data = msg.cdata;
  View v = data.getView();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgNewViewAcc>>::iterator it1 = this->newviewsAcc.find(v);
  if (it1 != this->newviewsAcc.end()) { // there is already an entry for this view
    std::set<MsgNewViewAcc> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgNewViewAccFrom(msgs,signer)) {
      msgs.insert(msg);
      this->newviewsAcc[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->newviewsAcc[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


std::set<MsgNewViewAcc> Log::getNewViewAcc(View view, unsigned int n) {
  std::set<MsgNewViewAcc> ret;
  std::map<View,std::set<MsgNewViewAcc>>::iterator it1 = this->newviewsAcc.find(view);
  if (it1 != this->newviewsAcc.end()) { // there is already an entry for this view
    std::set<MsgNewViewAcc> msgs = it1->second;
    for (std::set<MsgNewViewAcc>::iterator it=msgs.begin(); ret.size() < n && it!=msgs.end(); ++it) {
      MsgNewViewAcc msg = (MsgNewViewAcc)*it;
      ret.insert(msg);
    }
  }
  return ret;
}


unsigned int Log::storePrepAcc(MsgPrepareAcc msg) {
  CData<Hash,Void> data = msg.cdata;
  View v = data.getView();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgPrepareAcc>>::iterator it1 = this->preparesAcc.find(v);
  if (it1 != this->preparesAcc.end()) { // there is already an entry for this view
    std::set<MsgPrepareAcc> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgPrepareAccFrom(msgs,signers)) {
      msgs.insert(msg);
      this->preparesAcc[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->preparesAcc[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storeLdrPrepAcc(MsgLdrPrepareAcc msg) {
  CData<Block,Accum> data = msg.cdata;
  View v = data.getView();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgLdrPrepareAcc>>::iterator it1 = this->ldrpreparesAcc.find(v);
  if (it1 != this->ldrpreparesAcc.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareAcc> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgLdrPrepareAccFrom(msgs,signer)) {
      msgs.insert(msg);
      this->ldrpreparesAcc[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->ldrpreparesAcc[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


Signs Log::getPrepareAcc(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPrepareAcc>>::iterator it1 = this->preparesAcc.find(view);
  if (it1 != this->preparesAcc.end()) { // there is already an entry for this view
    std::set<MsgPrepareAcc> msgs = it1->second;
    for (std::set<MsgPrepareAcc>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPrepareAcc msg = (MsgPrepareAcc)*it;
      Signs others = msg.signs;
      signs.addUpto(others,n);
    }
  }
  return signs;
}


unsigned int Log::storePcAcc(MsgPreCommitAcc msg) {
  CData<Hash,Void> data = msg.cdata;
  View v = data.getView();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgPreCommitAcc>>::iterator it1 = this->precommitsAcc.find(v);
  if (it1 != this->precommitsAcc.end()) { // there is already an entry for this view
    std::set<MsgPreCommitAcc> msgs = it1->second;

    if (!msgPreCommitAccFrom(msgs,signers)) {
      msgs.insert(msg);
      this->precommitsAcc[v]=msgs;
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->precommitsAcc[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


Signs Log::getPrecommitAcc(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPreCommitAcc>>::iterator it1 = this->precommitsAcc.find(view);
  if (it1 != this->precommitsAcc.end()) { // there is already an entry for this view
    std::set<MsgPreCommitAcc> msgs = it1->second;
    for (std::set<MsgPreCommitAcc>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPreCommitAcc msg = (MsgPreCommitAcc)*it;
      Signs others = msg.signs;
      signs.addUpto(others,n);
    }
  }
  return signs;
}


unsigned int Log::storeNvComb(MsgNewViewComb msg) {
  RData data = msg.data;
  View v = data.getPropv();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgNewViewComb>>::iterator it1 = this->newviewsComb.find(v);
  if (it1 != this->newviewsComb.end()) { // there is already an entry for this view
    std::set<MsgNewViewComb> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgNewViewCombFrom(msgs,signer)) {
      msgs.insert(msg);
      this->newviewsComb[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->newviewsComb[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


std::set<MsgNewViewComb> Log::getNewViewComb(View view, unsigned int n) {
  std::set<MsgNewViewComb> ret;
  std::map<View,std::set<MsgNewViewComb>>::iterator it1 = this->newviewsComb.find(view);
  if (it1 != this->newviewsComb.end()) { // there is already an entry for this view
    std::set<MsgNewViewComb> msgs = it1->second;
    for (std::set<MsgNewViewComb>::iterator it=msgs.begin(); ret.size() < n && it!=msgs.end(); ++it) {
      MsgNewViewComb msg = (MsgNewViewComb)*it;
      ret.insert(msg);
    }
  }
  return ret;
}


unsigned int Log::storePrepComb(MsgPrepareComb msg) {
  RData data = msg.data;
  View v = data.getPropv();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgPrepareComb>>::iterator it1 = this->preparesComb.find(v);
  if (it1 != this->preparesComb.end()) { // there is already an entry for this view
    std::set<MsgPrepareComb> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgPrepareCombFrom(msgs,signers)) {
      msgs.insert(msg);
      this->preparesComb[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->preparesComb[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storeLdrPrepComb(MsgLdrPrepareComb msg) {
  Accum acc = msg.acc;
  View v = acc.getView();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgLdrPrepareComb>>::iterator it1 = this->ldrpreparesComb.find(v);
  if (it1 != this->ldrpreparesComb.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareComb> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgLdrPrepareCombFrom(msgs,signer)) {
      msgs.insert(msg);
      this->ldrpreparesComb[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->ldrpreparesComb[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


Signs Log::getPrepareComb(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPrepareComb>>::iterator it1 = this->preparesComb.find(view);
  if (it1 != this->preparesComb.end()) { // there is already an entry for this view
    std::set<MsgPrepareComb> msgs = it1->second;
    for (std::set<MsgPrepareComb>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPrepareComb msg = (MsgPrepareComb)*it;
      Signs others = msg.signs;
      signs.addUpto(others,n);
    }
  }
  return signs;
}


unsigned int Log::storePcComb(MsgPreCommitComb msg) {
  RData data = msg.data;
  View v = data.getPropv();
  std::set<PID> signers = msg.signs.getSigners();

  std::map<View,std::set<MsgPreCommitComb>>::iterator it1 = this->precommitsComb.find(v);
  if (it1 != this->precommitsComb.end()) { // there is already an entry for this view
    std::set<MsgPreCommitComb> msgs = it1->second;

    if (!msgPreCommitCombFrom(msgs,signers)) {
      msgs.insert(msg);
      this->precommitsComb[v]=msgs;
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->precommitsComb[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


Signs Log::getPrecommitComb(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPreCommitComb>>::iterator it1 = this->precommitsComb.find(view);
  if (it1 != this->precommitsComb.end()) { // there is already an entry for this view
    std::set<MsgPreCommitComb> msgs = it1->second;
    for (std::set<MsgPreCommitComb>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPreCommitComb msg = (MsgPreCommitComb)*it;
      Signs others = msg.signs;
      signs.addUpto(others,n);
    }
  }
  return signs;
}

unsigned int Log::storeNvFree(MsgNewViewFree msg) {
  FData data = msg.data;
  View v = data.getView();
  PID signer = msg.auth.getId();

  std::map<View,std::set<MsgNewViewFree>>::iterator it1 = this->newviewsFree.find(v);
  if (it1 != this->newviewsFree.end()) { // there is already an entry for this view
    std::set<MsgNewViewFree> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgNewViewFreeFrom(msgs,signer)) {
      msgs.insert(msg);
      this->newviewsFree[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }

    //return msgs.size();
  } else { // there is no entry for this view
    this->newviewsFree[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


std::set<MsgNewViewFree> Log::getNewViewFree(View view, unsigned int n) {
  std::set<MsgNewViewFree> ret;
  std::map<View,std::set<MsgNewViewFree>>::iterator it1 = this->newviewsFree.find(view);
  if (it1 != this->newviewsFree.end()) { // there is already an entry for this view
    std::set<MsgNewViewFree> msgs = it1->second;
    for (std::set<MsgNewViewFree>::iterator it=msgs.begin(); ret.size() < n && it!=msgs.end(); ++it) {
      MsgNewViewFree msg = (MsgNewViewFree)*it;
      ret.insert(msg);
    }
  }
  return ret;
}


unsigned int Log::storePrepFree(PJust msg) {
  View v = msg.getView();

  std::map<View,std::tuple<PJust>>::iterator it1 = this->preparesFree.find(v);
  if (it1 != this->preparesFree.end()) { // there is already an entry for this view
    if (DEBUG1) { std::cout << KGRN << "updating prep for view (" << v << ")" << KNRM << std::endl; }
    // We update the entry with the new info
    PJust prep = std::get<0>(it1->second);
    PJust just(msg.getHash(),msg.getView(),msg.getAuth(),prep.getAuths());
    it1->second = std::make_tuple(just);
    return prep.sizeAuth();
  } else { // there is no entry for this view
    if (DEBUG1) { std::cout << KGRN << "storing prep for view (" << v << ")" << KNRM << std::endl; }
    this->preparesFree[v] = std::make_tuple(msg);
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}



unsigned int Log::storeBckPrepFree(MsgBckPrepareFree msg) {
  if (DEBUG1) std::cout << KLBLU << "storeBckPrepFree-0" << KNRM << std::endl;
  View v = msg.view;
  PID signer = msg.auth.getId();

  std::map<View,std::tuple<PJust>>::iterator it1 = this->preparesFree.find(v);
  if (it1 != this->preparesFree.end()) { // there is already an entry for this view
    PJust prep = std::get<0>(it1->second);
    if (DEBUG1) std::cout << KLBLU << "storeBckPrepFree-1" << KNRM << std::endl;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgPrepareFreeFrom(prep,signer)) {
      //Auths auths = prep.auths;
      //auths.add(msg.auth);
      //MsgPrepareFree newprep = MsgPrepareFree(prep.hash,prep.view,prep.auth,auths);
      //it1->second = newprep;
      //this->preparesFree[v].setAuths(auths);
      if (DEBUG1) { std::cout << KGRN << "updating bck prep for view (" << v << ")" << KNRM << std::endl; }
      prep.add(msg.auth);
      it1->second = std::make_tuple(prep);
      unsigned int s = prep.sizeAuth();
      if (DEBUG1) std::cout << KLBLU << "storeBckPrepFree-4(" << s << ")" << KNRM << std::endl;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return s;
    }
  } else { // there is no entry for this view
    PJust newprep = PJust(Hash(false),msg.view,Auth(false),Auths(msg.auth));
    if (DEBUG1) { std::cout << KGRN << "storing bck prep for view (" << v << ")" << KNRM << std::endl; }
    this->preparesFree[v] = std::make_tuple(newprep);
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storeLdrPrepFree(HAccum msg) {
  View v = msg.getView();

  std::map<View,std::tuple<HAccum>>::iterator it1 = this->ldrpreparesFree.find(v);
  if (it1 != this->ldrpreparesFree.end()) { // there is already an entry for this view
    return 1;
  } else { // there is no entry for this view
    this->ldrpreparesFree[v] = std::make_tuple(msg);
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


PJust Log::getPrepareFree(View view) {
  std::map<View,std::tuple<PJust>>::iterator it1 = this->preparesFree.find(view);
  if (it1 != this->preparesFree.end()) { // there is already an entry for this view
    return std::get<0>(it1->second);
  }
  PJust msg(Hash(false),0,Auth(false),Auths());
  return msg;
}


unsigned int Log::storePcFree(MsgPreCommitFree msg) {
  View v = msg.view;
  std::set<PID> signers = msg.auths.getSigners();

  std::map<View,std::set<MsgPreCommitFree>>::iterator it1 = this->precommitsFree.find(v);
  if (it1 != this->precommitsFree.end()) { // there is already an entry for this view
    std::set<MsgPreCommitFree> msgs = it1->second;

    if (!msgPreCommitFreeFrom(msgs,signers)) {
      msgs.insert(msg);
      this->precommitsFree[v]=msgs;
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->precommitsFree[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


Auths Log::getPrecommitFree(View view, unsigned int n) {
  Auths auths;
  std::map<View,std::set<MsgPreCommitFree>>::iterator it1 = this->precommitsFree.find(view);
  if (it1 != this->precommitsFree.end()) { // there is already an entry for this view
    std::set<MsgPreCommitFree> msgs = it1->second;
    for (std::set<MsgPreCommitFree>::iterator it=msgs.begin(); auths.getSize() < n && it!=msgs.end(); ++it) {
      MsgPreCommitFree msg = (MsgPreCommitFree)*it;
      Auths others = msg.auths;
      auths.addUpto(others,n);
    }
  }
  return auths;
}


///////////////////



unsigned int Log::storeNvOp(OPprepare prep) {
  View v = prep.getView();
//  PID signer = prep.auth.getId();

  std::map<View,std::set<OPprepare>>::iterator it1 = this->newviewsOPa.find(v);
  if (it1 != this->newviewsOPa.end()) { // there is already an entry for this view
    std::set<OPprepare> msgs = it1->second;

    // if there's already a stored prepared message, we don't do anything
    if (msgs.size() == 0) {
      msgs.insert(prep);
      this->newviewsOPa[v]=msgs;
    }
    return msgs.size();

  } else { // there is no entry for this view
    this->newviewsOPa[v]={prep};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


bool msgNewViewOpFrom(std::set<OPnvcert> certs, PID i, View x) {
  for (std::set<OPnvcert>::iterator it=certs.begin(); it!=certs.end(); ++it) {
    OPnvcert cert = (OPnvcert)*it;
    if (i == cert.store.getAuth().getId()) { return true; }
  }
  return false;
}


unsigned int Log::storeNvOp(OPnvblock newnv) {
  View v = newnv.cert.store.getView();
  PID  i = newnv.cert.store.getAuth().getId();
  View x = newnv.cert.store.getV();

  std::map<View,std::set<OPnvblocks>>::iterator it1 = this->newviewsOPb.find(v);
  if (it1 != this->newviewsOPb.end()) { // there is already an entry for this view
    std::set<OPnvblocks> msgs = it1->second;

    // if there is an entry, there should be only one
    std::set<OPnvblocks>::iterator it2 = msgs.begin();
    if (it2 != msgs.end()) {
      OPnvblocks nv = (OPnvblocks)*it2;

      if (DEBUG1) { std::cout << KGRN << "current nvblocks:" << nv.prettyPrint() << KNRM << std::endl; }
      if (DEBUG1) { std::cout << KGRN << "adding nvblock:" << newnv.prettyPrint() << KNRM << std::endl; }

      // if there's already a stored prepared message form i, we don't do anything
      if (i != nv.nv.cert.store.getAuth().getId() && !msgNewViewOpFrom(nv.certs.certs,i,x)) {
        if (x > nv.nv.cert.store.getV()) {
          // a new highest
          nv.new_block(newnv);
        } else {
          // not a new highest
          nv.insert(newnv.cert);
        }

        //msgs.insert(newnv);
        this->newviewsOPb[v]={nv};
      }
      return (1 + nv.certs.certs.size());

    }

  } else { // there is no entry for this view
    this->newviewsOPb[v]={OPnvblocks(newnv)};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


OPnvblocks Log::getNvOpbs(View view) {
  //std::set<OPnvblock> ret;
  std::map<View,std::set<OPnvblocks>>::iterator it1 = this->newviewsOPb.find(view);
  if (it1 != this->newviewsOPb.end()) { // there is an entry for this view
    std::set<OPnvblocks> msgs = it1->second;

    // if there is an entry, there should be only one
    std::set<OPnvblocks>::iterator it2 = msgs.begin();
    if (it2 != msgs.end()) {
      return (OPnvblocks)*it2;
    }

  }

  return OPnvblocks();
}


OPprepare Log::getNvOpas(View view) {
  //std::set<OPnvblock> ret;
  std::map<View,std::set<OPprepare>>::iterator it1 = this->newviewsOPa.find(view);
  if (it1 != this->newviewsOPa.end()) { // there is an entry for this view
    std::set<OPprepare> msgs = it1->second;

    // if there is an entry, there should be only one
    std::set<OPprepare>::iterator it2 = msgs.begin();
    if (it2 != msgs.end()) {
      return (OPprepare)*it2;
    }

  }

  return OPprepare();
}


bool OPstoreFrom(std::set<OPstore> msgs, PID signer) {
  for (std::set<OPstore>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    OPstore msg = (OPstore)*it;
    PID k = msg.getAuth().getId();
    if (signer == k) { return true; }
  }
  return false;
}


unsigned int Log::storeStoreOp(OPstore msg) {
  View v = msg.getView();

  std::map<View,std::set<OPstore>>::iterator it1 = this->storesOP.find(v);
  if (it1 != this->storesOP.end()) { // there is already an entry for this view
    std::set<OPstore> msgs = it1->second;

    if (!OPstoreFrom(msgs,msg.getAuth().getId())) {
      msgs.insert(msg);
      this->storesOP[v]=msgs;
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->storesOP[v]={msg};
    if (DEBUG1) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


OPprepare Log::getOPstores(View view, unsigned int n) {
  OPprepare ret;
  std::map<View,std::set<OPstore>>::iterator it1 = this->storesOP.find(view);
  if (it1 != this->storesOP.end()) { // there is already an entry for this view
    std::set<OPstore> msgs = it1->second;
    //if (DEBUG1) { std::cout << KGRN << "making prepare (" << msgs.size() << ")" << KNRM << std::endl; }
    for (std::set<OPstore>::iterator it=msgs.begin(); ret.getAuths().getSize() < n && it!=msgs.end(); ++it) {
      OPstore msg = (OPstore)*it;
      //if (DEBUG1) { std::cout << KGRN << "store:" << msg.getHash().toString() << KNRM << std::endl; }
      ret.insert(msg);
    }
  }
  return ret;
}


unsigned int Log::storeLdrPrepOp(LdrPrepareOP msg) {
  View v = msg.prop.getView();

  std::map<View,std::tuple<LdrPrepareOP>>::iterator it1 = this->ldrpreparesOP.find(v);
  if (it1 != this->ldrpreparesOP.end()) { // there is already an entry for this view
    return 1;
  } else { // there is no entry for this view
    this->ldrpreparesOP[v] = std::make_tuple(msg);
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


LdrPrepareOP Log::getLdrPrepareOp(View view) {
  std::map<View,std::tuple<LdrPrepareOP>>::iterator it = this->ldrpreparesOP.find(view);
  if (it != this->ldrpreparesOP.end()) { // there is already an entry for this view
    LdrPrepareOP msg = std::get<0>(it->second);
    return msg;
  }
  LdrPrepareOP msg; //(HAccum(),Block());
  return msg;
}


unsigned int Log::storePrepareOp(OPprepare msg) {
  View v = msg.getView();

  std::map<View,std::set<OPprepare>>::iterator it1 = this->preparesOP.find(v);
  if (it1 != this->preparesOP.end()) { // there is already an entry for this view
    std::set<OPprepare> msgs = it1->second;

    if (msgs.size() == 0) {
      msgs.insert(msg);
      this->preparesOP[v]=msgs;
      return msgs.size();
    }

  } else { // there is no entry for this view
    this->preparesOP[v]={msg};
    if (DEBUG1) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


OPprepare Log::getOPprepare(View view) {
  OPprepare ret;
  std::map<View,std::set<OPprepare>>::iterator it1 = this->preparesOP.find(view);
  if (it1 != this->preparesOP.end()) { // there is already an entry for this view
    std::set<OPprepare> msgs = it1->second;
    if (msgs.size() > 0) {
      std::set<OPprepare>::iterator it=msgs.begin();
      ret = (OPprepare)*it;
    }
  }
  return ret;
}


void Log::storeAccumOp(OPaccum acc) {
  View v = acc.getView();

  std::map<View,std::set<OPaccum>>::iterator it1 = this->accumsOP.find(v);
  if (it1 != this->accumsOP.end()) { // there is already an entry for this view
    std::set<OPaccum> accs = it1->second;

    // We only store 1 value
    if (accs.size() == 0) {
      accs.insert(acc);
      this->accumsOP[v]=accs;
    }

  } else { // there is no entry for this view
    this->accumsOP[v]={acc};
    if (DEBUG1) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
  }
}


OPaccum Log::getAccOp(View view) {
  OPaccum ret;
  std::map<View,std::set<OPaccum>>::iterator it1 = this->accumsOP.find(view);
  if (it1 != this->accumsOP.end()) { // there is already an entry for this view
    std::set<OPaccum> accs = it1->second;
    if (accs.size() > 0) {
      std::set<OPaccum>::iterator it=accs.begin();
      ret = (OPaccum)*it;
    }
  }
  return ret;
}


unsigned int Log::storeVoteOp(OPvote msg, unsigned int* m) {
  View v = msg.getView();

  std::map<View,std::set<OPvote>>::iterator it1 = this->votesOP.find(v);
  if (it1 != this->votesOP.end()) { // there is already an entry for this view
    std::set<OPvote> msgs = it1->second;
    if (DEBUG1) { std::cout << KGRN << "storeVoteOp:found an entry for this view (" << v << "):" << msgs.size() << KNRM << std::endl; }

    std::set<OPvote>::iterator it2 = msgs.begin();
    while (it2 != msgs.end()) {
      OPvote vote = (OPvote)*it2;
      if (msg.getHash() == vote.getHash()) {
        *m = vote.getAuths().getSize();
        msgs.erase(it2);
        if (DEBUG1) { std::cout << KGRN << "storeVoteOp:votes:" << vote.getAuths().prettyPrint() << ";" << msg.getAuths().prettyPrint() << KNRM << std::endl; }
        vote.insert(msg.getAuths());
        msgs.insert(vote);
        this->votesOP[v]=msgs;
        unsigned int n = vote.getAuths().getSize();
        return n;
      }
    }

    return 0;

  } else { // there is no entry for this view
    this->votesOP[v]={msg};
    if (DEBUG1) { std::cout << KGRN << "storeVoteOp:no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return msg.getAuths().getSize();
  }

  return 0;
}


OPvote Log::getOPvote(View view, unsigned int n) {
  std::map<View,std::set<OPvote>>::iterator it1 = this->votesOP.find(view);
  if (it1 != this->votesOP.end()) { // there is already an entry for this view
    std::set<OPvote> msgs = it1->second;
    std::set<OPvote>::iterator it2 = msgs.begin();
    while (it2 != msgs.end()) {
      OPvote vote = (OPvote)*it2;
      if (vote.getAuths().getSize() >= n) {
        return vote;
      }
    }
  }
  return OPvote();
}



///////////////////



bool msgNewViewChFrom(std::set<MsgNewViewCh> msgs, PID signer) {
  for (std::set<MsgNewViewCh>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgNewViewCh msg = (MsgNewViewCh)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgPrepareChFrom(std::set<MsgPrepareCh> msgs, PID signer) {
  for (std::set<MsgPrepareCh>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPrepareCh msg = (MsgPrepareCh)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgLdrPrepareChFrom(std::set<MsgLdrPrepareCh> msgs, PID signer) {
  for (std::set<MsgLdrPrepareCh>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgLdrPrepareCh msg = (MsgLdrPrepareCh)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

unsigned int Log::storeNvCh(MsgNewViewCh msg) {
  RData data = msg.data;
  View v = data.getPropv();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgNewViewCh>>::iterator it1 = this->newviewsCh.find(v);
  if (it1 != this->newviewsCh.end()) { // there is already an entry for this view
    std::set<MsgNewViewCh> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgNewViewChFrom(msgs,signer)) {
      msgs.insert(msg);
      this->newviewsCh[v]=msgs;
      if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->newviewsCh[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


std::set<MsgNewViewCh> Log::getNewViewCh(View view, unsigned int n) {
  std::set<MsgNewViewCh> ret;
  std::map<View,std::set<MsgNewViewCh>>::iterator it1 = this->newviewsCh.find(view);
  if (it1 != this->newviewsCh.end()) { // there is already an entry for this view
    std::set<MsgNewViewCh> msgs = it1->second;
    for (std::set<MsgNewViewCh>::iterator it=msgs.begin(); ret.size() < n && it!=msgs.end(); ++it) {
      MsgNewViewCh msg = (MsgNewViewCh)*it;
      ret.insert(msg);
    }
  }
  return ret;
}


unsigned int Log::storePrepCh(MsgPrepareCh msg) {
  RData data = msg.data;
  View v = data.getPropv();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgPrepareCh>>::iterator it1 = this->preparesCh.find(v);
  if (it1 != this->preparesCh.end()) { // there is already an entry for this view
    std::set<MsgPrepareCh> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgPrepareChFrom(msgs,signer)) {
      msgs.insert(msg);
      this->preparesCh[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->preparesCh[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storeLdrPrepCh(MsgLdrPrepareCh msg) {
  JBlock block = msg.block;
  View v = block.getView(); //getJust().getRData().getPropv();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgLdrPrepareCh>>::iterator it1 = this->ldrpreparesCh.find(v);
  if (it1 != this->ldrpreparesCh.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareCh> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgLdrPrepareChFrom(msgs,signer)) {
      msgs.insert(msg);
      this->ldrpreparesCh[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->ldrpreparesCh[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


Signs Log::getPrepareCh(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPrepareCh>>::iterator it1 = this->preparesCh.find(view);
  if (it1 != this->preparesCh.end()) { // there is already an entry for this view
    std::set<MsgPrepareCh> msgs = it1->second;
    for (std::set<MsgPrepareCh>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPrepareCh msg = (MsgPrepareCh)*it;
      Signs others = Signs(msg.sign);
      signs.addUpto(others,n);
    }
  }
  return signs;
}

Just Log::findHighestNvCh(View view) {
  std::map<View,std::set<MsgNewViewCh>>::iterator it1 = this->newviewsCh.find(view);
  Just just = Just();
  if (it1 != this->newviewsCh.end()) { // there is already an entry for this view
    std::set<MsgNewViewCh> msgs = it1->second;
    View h = 0;
    for (std::set<MsgNewViewCh>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
      MsgNewViewCh msg = (MsgNewViewCh)*it;
      RData data = msg.data;
      Sign sign = msg.sign;
      View v = data.getJustv();
      if (v >= h) { h = v; just = Just(data,{sign}); }
    }
  }
  return just;
}


MsgLdrPrepareCh Log::firstLdrPrepareCh(View view) {
  std::map<View,std::set<MsgLdrPrepareCh>>::iterator it = this->ldrpreparesCh.find(view);
  if (it != this->ldrpreparesCh.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareCh> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgLdrPrepareCh)*(msgs.begin());
    }
  }
  return MsgLdrPrepareCh(JBlock(),Sign());
}

////////////////





///////////////////

bool msgNewViewChCombFrom(std::set<MsgNewViewChComb> msgs, PID signer) {
  for (std::set<MsgNewViewChComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgNewViewChComb msg = (MsgNewViewChComb)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgPrepareChCombFrom(std::set<MsgPrepareChComb> msgs, PID signer) {
  for (std::set<MsgPrepareChComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgPrepareChComb msg = (MsgPrepareChComb)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

bool msgLdrPrepareChCombFrom(std::set<MsgLdrPrepareChComb> msgs, PID signer) {
  for (std::set<MsgLdrPrepareChComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
    MsgLdrPrepareChComb msg = (MsgLdrPrepareChComb)*it;
    PID k = msg.sign.getSigner();
    if (signer == k) { return true; }
  }
  return false;
}

unsigned int Log::storeNvChComb(MsgNewViewChComb msg) {
  RData data = msg.data;
  View v = data.getPropv();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgNewViewChComb>>::iterator it1 = this->newviewsChComb.find(v);
  if (it1 != this->newviewsChComb.end()) { // there is already an entry for this view
    std::set<MsgNewViewChComb> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgNewViewChCombFrom(msgs,signer)) {
      msgs.insert(msg);
      this->newviewsChComb[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
      //k[hdr]=msgs;
      //log[v]=k;
    }
  } else { // there is no entry for this view
    this->newviewsChComb[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


std::set<MsgNewViewChComb> Log::getNewViewChComb(View view, unsigned int n) {
  std::set<MsgNewViewChComb> ret;
  std::map<View,std::set<MsgNewViewChComb>>::iterator it1 = this->newviewsChComb.find(view);
  if (it1 != this->newviewsChComb.end()) { // there is already an entry for this view
    std::set<MsgNewViewChComb> msgs = it1->second;
    for (std::set<MsgNewViewChComb>::iterator it=msgs.begin(); ret.size() < n && it!=msgs.end(); ++it) {
      MsgNewViewChComb msg = (MsgNewViewChComb)*it;
      ret.insert(msg);
    }
  }
  return ret;
}


unsigned int Log::storePrepChComb(MsgPrepareChComb msg) {
  RData data = msg.data;
  View v = data.getPropv();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgPrepareChComb>>::iterator it1 = this->preparesChComb.find(v);
  if (it1 != this->preparesChComb.end()) { // there is already an entry for this view
    std::set<MsgPrepareChComb> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgPrepareChCombFrom(msgs,signer)) {
      msgs.insert(msg);
      this->preparesChComb[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->preparesChComb[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


unsigned int Log::storeLdrPrepChComb(MsgLdrPrepareChComb msg) {
  CBlock block = msg.block;
  View v = block.getView();
  PID signer = msg.sign.getSigner();

  std::map<View,std::set<MsgLdrPrepareChComb>>::iterator it1 = this->ldrpreparesChComb.find(v);
  if (it1 != this->ldrpreparesChComb.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareChComb> msgs = it1->second;

    // We only add 'msg' to the log if the sender hasn't already sent a new-view message for this view
    if (!msgLdrPrepareChCombFrom(msgs,signer)) {
      msgs.insert(msg);
      this->ldrpreparesChComb[v]=msgs;
      //if (DEBUG) { std::cout << KGRN << "updated entry; #=" << msgs.size() << KNRM << std::endl; }
      return msgs.size();
    }
  } else { // there is no entry for this view
    this->ldrpreparesChComb[v]={msg};
    if (DEBUG) { std::cout << KGRN << "no entry for this view (" << v << ") before; #=1" << KNRM << std::endl; }
    return 1;
  }

  return 0;
}


Signs Log::getPrepareChComb(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPrepareChComb>>::iterator it1 = this->preparesChComb.find(view);
  if (it1 != this->preparesChComb.end()) { // there is already an entry for this view
    std::set<MsgPrepareChComb> msgs = it1->second;
    for (std::set<MsgPrepareChComb>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPrepareChComb msg = (MsgPrepareChComb)*it;
      Signs others = Signs(msg.sign);
      signs.addUpto(others,n);
    }
  }
  return signs;
}

Just Log::findHighestNvChComb(View view) {
  std::map<View,std::set<MsgNewViewChComb>>::iterator it1 = this->newviewsChComb.find(view);
  Just just = Just();
  if (it1 != this->newviewsChComb.end()) { // there is already an entry for this view
    std::set<MsgNewViewChComb> msgs = it1->second;
    View h = 0;
    for (std::set<MsgNewViewChComb>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
      MsgNewViewChComb msg = (MsgNewViewChComb)*it;
      RData data = msg.data;
      Sign sign = msg.sign;
      View v = data.getJustv();
      if (v >= h) { h = v; just = Just(data,{sign}); }
    }
  }
  return just;
}


MsgLdrPrepareChComb Log::firstLdrPrepareChComb(View view) {
  std::map<View,std::set<MsgLdrPrepareChComb>>::iterator it = this->ldrpreparesChComb.find(view);
  if (it != this->ldrpreparesChComb.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareChComb> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgLdrPrepareChComb)*(msgs.begin());
    }
  }
  return MsgLdrPrepareChComb(CBlock(),Sign());
}

////////////////




std::string Log::prettyPrint() {
  std::string text;
  // newviews
  for (std::map<View,std::set<MsgNewView>>::iterator it1=this->newviews.begin(); it1!=this->newviews.end();++it1) {
    View v = it1->first;
    std::set<MsgNewView> msgs = it1->second;
    text += "newviews;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //prepares
  for (std::map<View,std::set<MsgPrepare>>::iterator it1=this->prepares.begin(); it1!=this->prepares.end();++it1) {
    View v = it1->first;
    std::set<MsgPrepare> msgs = it1->second;
    text += "prepares;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //precommits
  for (std::map<View,std::set<MsgPreCommit>>::iterator it1=this->precommits.begin(); it1!=this->precommits.end();++it1) {
    View v = it1->first;
    std::set<MsgPreCommit> msgs = it1->second;
    text += "precommits;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //commits
  for (std::map<View,std::set<MsgCommit>>::iterator it1=this->commits.begin(); it1!=this->commits.end();++it1) {
    View v = it1->first;
    std::set<MsgCommit> msgs = it1->second;
    text += "commits;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //proposals
  for (std::map<View,std::set<MsgLdrPrepare>>::iterator it1=this->proposals.begin(); it1!=this->proposals.end();++it1) {
    View v = it1->first;
    std::set<MsgLdrPrepare> msgs = it1->second;
    text += "proposals;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }

  //newviewsAcc
  for (std::map<View,std::set<MsgNewViewAcc>>::iterator it1=this->newviewsAcc.begin(); it1!=this->newviewsAcc.end();++it1) {
    View v = it1->first;
    std::set<MsgNewViewAcc> msgs = it1->second;
    text += "newviews-acc;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //preparesAcc
  for (std::map<View,std::set<MsgPrepareAcc>>::iterator it1=this->preparesAcc.begin(); it1!=this->preparesAcc.end();++it1) {
    View v = it1->first;
    std::set<MsgPrepareAcc> msgs = it1->second;
    text += "prepares-acc;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //ldrpreparesAcc
  for (std::map<View,std::set<MsgLdrPrepareAcc>>::iterator it1=this->ldrpreparesAcc.begin(); it1!=this->ldrpreparesAcc.end();++it1) {
    View v = it1->first;
    std::set<MsgLdrPrepareAcc> msgs = it1->second;
    text += "pre-prepares-acc;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //precommitsAcc
  for (std::map<View,std::set<MsgPreCommitAcc>>::iterator it1=this->precommitsAcc.begin(); it1!=this->precommitsAcc.end();++it1) {
    View v = it1->first;
    std::set<MsgPreCommitAcc> msgs = it1->second;
    text += "precommits-acc;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }

  //newviewsComb
  for (std::map<View,std::set<MsgNewViewComb>>::iterator it1=this->newviewsComb.begin(); it1!=this->newviewsComb.end();++it1) {
    View v = it1->first;
    std::set<MsgNewViewComb> msgs = it1->second;
    text += "newviews-comb;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //preparesComb
  for (std::map<View,std::set<MsgPrepareComb>>::iterator it1=this->preparesComb.begin(); it1!=this->preparesComb.end();++it1) {
    View v = it1->first;
    std::set<MsgPrepareComb> msgs = it1->second;
    text += "prepares-comb;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //ldrpreparesComb
  for (std::map<View,std::set<MsgLdrPrepareComb>>::iterator it1=this->ldrpreparesComb.begin(); it1!=this->ldrpreparesComb.end();++it1) {
    View v = it1->first;
    std::set<MsgLdrPrepareComb> msgs = it1->second;
    text += "pre-prepares-comb;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //precommitsComb
  for (std::map<View,std::set<MsgPreCommitComb>>::iterator it1=this->precommitsComb.begin(); it1!=this->precommitsComb.end();++it1) {
    View v = it1->first;
    std::set<MsgPreCommitComb> msgs = it1->second;
    text += "precommits-comb;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }

  //newviewsCh
  for (std::map<View,std::set<MsgNewViewCh>>::iterator it1=this->newviewsCh.begin(); it1!=this->newviewsCh.end();++it1) {
    View v = it1->first;
    std::set<MsgNewViewCh> msgs = it1->second;
    text += "newviews-ch;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //preparesCh
  for (std::map<View,std::set<MsgPrepareCh>>::iterator it1=this->preparesCh.begin(); it1!=this->preparesCh.end();++it1) {
    View v = it1->first;
    std::set<MsgPrepareCh> msgs = it1->second;
    text += "prepares-ch;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //ldrpreparesCh
  for (std::map<View,std::set<MsgLdrPrepareCh>>::iterator it1=this->ldrpreparesCh.begin(); it1!=this->ldrpreparesCh.end();++it1) {
    View v = it1->first;
    std::set<MsgLdrPrepareCh> msgs = it1->second;
    text += "pre-prepares-ch;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }

  //newviewsChComb
  for (std::map<View,std::set<MsgNewViewChComb>>::iterator it1=this->newviewsChComb.begin(); it1!=this->newviewsChComb.end();++it1) {
    View v = it1->first;
    std::set<MsgNewViewChComb> msgs = it1->second;
    text += "newviews-ch-comb;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //preparesChComb
  for (std::map<View,std::set<MsgPrepareChComb>>::iterator it1=this->preparesChComb.begin(); it1!=this->preparesChComb.end();++it1) {
    View v = it1->first;
    std::set<MsgPrepareChComb> msgs = it1->second;
    text += "prepares-ch-comb;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }
  //ldrpreparesChComb
  for (std::map<View,std::set<MsgLdrPrepareChComb>>::iterator it1=this->ldrpreparesChComb.begin(); it1!=this->ldrpreparesChComb.end();++it1) {
    View v = it1->first;
    std::set<MsgLdrPrepareChComb> msgs = it1->second;
    text += "pre-prepares-ch-comb;view=" + std::to_string(v) + ";#=" + std::to_string(msgs.size()) + "\n";
  }

  return text;
}


Just Log::findHighestNv(View view) {
  std::map<View,std::set<MsgNewView>>::iterator it1 = this->newviews.find(view);
  Just just = Just();
  if (it1 != this->newviews.end()) { // there is already an entry for this view
    std::set<MsgNewView> msgs = it1->second;
    View h = 0;
    for (std::set<MsgNewView>::iterator it=msgs.begin(); it!=msgs.end(); ++it) {
      MsgNewView msg = (MsgNewView)*it;
      RData rdata = msg.rdata;
      Signs signs = msg.signs;
      View v = rdata.getJustv();
      if (v >= h) { h = v; just = Just(rdata,signs); }
    }
  }
  return just;
}


Just Log::firstPrepare(View view) {
  std::map<View,std::set<MsgPrepare>>::iterator it1 = this->prepares.find(view);
  Just just = Just();
  if (it1 != this->prepares.end()) { // there is already an entry for this view
    std::set<MsgPrepare> msgs = it1->second;
    if (0 < msgs.size()) { // We return the first element
      std::set<MsgPrepare>::iterator it=msgs.begin();
      MsgPrepare msg = (MsgPrepare)*it;
      RData rdata = msg.rdata;
      Signs signs = msg.signs;
      return Just(rdata,signs);
    }
  }
  return just;
}


Just Log::firstPrecommit(View view) {
  std::map<View,std::set<MsgPreCommit>>::iterator it1 = this->precommits.find(view);
  Just just = Just();
  if (it1 != this->precommits.end()) { // there is already an entry for this view
    std::set<MsgPreCommit> msgs = it1->second;
    if (0 < msgs.size()) { // We return the first element
      std::set<MsgPreCommit>::iterator it=msgs.begin();
      MsgPreCommit msg = (MsgPreCommit)*it;
      RData rdata = msg.rdata;
      Signs signs = msg.signs;
      return Just(rdata,signs);
    }
  }
  return just;
}


Just Log::firstCommit(View view) {
  std::map<View,std::set<MsgCommit>>::iterator it1 = this->commits.find(view);
  Just just = Just();
  if (it1 != this->commits.end()) { // there is already an entry for this view
    std::set<MsgCommit> msgs = it1->second;
    if (0 < msgs.size()) { // We return the first element
      std::set<MsgCommit>::iterator it=msgs.begin();
      MsgCommit msg = (MsgCommit)*it;
      RData rdata = msg.rdata;
      Signs signs = msg.signs;
      return Just(rdata,signs);
    }
  }
  return just;
}


MsgLdrPrepare Log::firstProposal(View view) {
  std::map<View,std::set<MsgLdrPrepare>>::iterator it1 = this->proposals.find(view);
  MsgLdrPrepare msg;
  if (it1 != this->proposals.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepare> msgs = it1->second;
    if (0 < msgs.size()) { // We return the first element
      std::set<MsgLdrPrepare>::iterator it=msgs.begin();
      MsgLdrPrepare msg = (MsgLdrPrepare)*it;
      return msg;
    }
  }
  return msg;
}



Signs Log::getNewView(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgNewView>>::iterator it1 = this->newviews.find(view);
  if (it1 != this->newviews.end()) { // there is already an entry for this view
    std::set<MsgNewView> msgs = it1->second;
    for (std::set<MsgNewView>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgNewView msg = (MsgNewView)*it;
      Signs others = msg.signs;
      //if (DEBUG) std::cout << KMAG << "adding-log-prep-signatures: " << others.prettyPrint() << KNRM << std::endl;
      signs.addUpto(others,n);
    }
  }
  //if (DEBUG) std::cout << KMAG << "log-prep-signatures: " << signs.prettyPrint() << KNRM << std::endl;
  return signs;
}

Signs Log::getPrepare(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPrepare>>::iterator it1 = this->prepares.find(view);
  if (it1 != this->prepares.end()) { // there is already an entry for this view
    std::set<MsgPrepare> msgs = it1->second;
    for (std::set<MsgPrepare>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPrepare msg = (MsgPrepare)*it;
      Signs others = msg.signs;
      //if (DEBUG) std::cout << KMAG << "adding-log-prep-signatures: " << others.prettyPrint() << KNRM << std::endl;
      signs.addUpto(others,n);
    }
  }
  //if (DEBUG) std::cout << KMAG << "log-prep-signatures: " << signs.prettyPrint() << KNRM << std::endl;
  return signs;
}


Signs Log::getPrecommit(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgPreCommit>>::iterator it1 = this->precommits.find(view);
  if (it1 != this->precommits.end()) { // there is already an entry for this view
    std::set<MsgPreCommit> msgs = it1->second;
    for (std::set<MsgPreCommit>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgPreCommit msg = (MsgPreCommit)*it;
      Signs others = msg.signs;
      //if (DEBUG) std::cout << KMAG << "adding-log-pc-signatures: " << others.prettyPrint() << KNRM << std::endl;
      signs.addUpto(others,n);
    }
  }
  return signs;
}

Signs Log::getCommit(View view, unsigned int n) {
  Signs signs;
  std::map<View,std::set<MsgCommit>>::iterator it1 = this->commits.find(view);
  if (it1 != this->commits.end()) { // there is already an entry for this view
    std::set<MsgCommit> msgs = it1->second;
    for (std::set<MsgCommit>::iterator it=msgs.begin(); signs.getSize() < n && it!=msgs.end(); ++it) {
      MsgCommit msg = (MsgCommit)*it;
      Signs others = msg.signs;
      //if (DEBUG) std::cout << KMAG << "adding-log-com-signatures: " << others.prettyPrint() << KNRM << std::endl;
      signs.addUpto(others,n);
    }
  }
  return signs;
}


MsgLdrPrepareAcc Log::firstLdrPrepareAcc(View view) {
  std::map<View,std::set<MsgLdrPrepareAcc>>::iterator it = this->ldrpreparesAcc.find(view);
  if (it != this->ldrpreparesAcc.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareAcc> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgLdrPrepareAcc)*(msgs.begin());
    }
  }
  CData<Block,Accum> cdata;
  Sign sign;
  MsgLdrPrepareAcc msg(cdata,sign);
  return msg;
}


MsgPrepareAcc Log::firstPrepareAcc(View view) {
  std::map<View,std::set<MsgPrepareAcc>>::iterator it = this->preparesAcc.find(view);
  if (it != this->preparesAcc.end()) { // there is already an entry for this view
    std::set<MsgPrepareAcc> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgPrepareAcc)*(msgs.begin());
    }
  }
  CData<Hash,Void> cdata;
  MsgPrepareAcc msg(cdata,{});
  return msg;
}


MsgPreCommitAcc Log::firstPrecommitAcc(View view) {
  std::map<View,std::set<MsgPreCommitAcc>>::iterator it = this->precommitsAcc.find(view);
  if (it != this->precommitsAcc.end()) { // there is already an entry for this view
    std::set<MsgPreCommitAcc> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgPreCommitAcc)*(msgs.begin());
    }
  }
  CData<Hash,Void> cdata;
  MsgPreCommitAcc msg(cdata,{});
  return msg;
}


MsgLdrPrepareComb Log::firstLdrPrepareComb(View view) {
  std::map<View,std::set<MsgLdrPrepareComb>>::iterator it = this->ldrpreparesComb.find(view);
  if (it != this->ldrpreparesComb.end()) { // there is already an entry for this view
    std::set<MsgLdrPrepareComb> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgLdrPrepareComb)*(msgs.begin());
    }
  }
  Accum acc;
  Block block;
  Sign sign;
  MsgLdrPrepareComb msg(acc,block,sign);
  return msg;
}


HAccum Log::getLdrPrepareFree(View view) {
  std::map<View,std::tuple<HAccum>>::iterator it = this->ldrpreparesFree.find(view);
  if (it != this->ldrpreparesFree.end()) { // there is already an entry for this view
    HAccum msg = std::get<0>(it->second);
    return msg;
  }
  HAccum msg; //(HAccum(),Block());
  return msg;
}


MsgPrepareComb Log::firstPrepareComb(View view) {
  std::map<View,std::set<MsgPrepareComb>>::iterator it = this->preparesComb.find(view);
  if (it != this->preparesComb.end()) { // there is already an entry for this view
    std::set<MsgPrepareComb> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgPrepareComb)*(msgs.begin());
    }
  }
  RData data;
  MsgPrepareComb msg(data,{});
  return msg;
}


MsgPreCommitComb Log::firstPrecommitComb(View view) {
  std::map<View,std::set<MsgPreCommitComb>>::iterator it = this->precommitsComb.find(view);
  if (it != this->precommitsComb.end()) { // there is already an entry for this view
    std::set<MsgPreCommitComb> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgPreCommitComb)*(msgs.begin());
    }
  }
  RData data;
  MsgPreCommitComb msg(data,{});
  return msg;
}

MsgPreCommitFree Log::firstPrecommitFree(View view) {
  std::map<View,std::set<MsgPreCommitFree>>::iterator it = this->precommitsFree.find(view);
  if (it != this->precommitsFree.end()) { // there is already an entry for this view
    std::set<MsgPreCommitFree> msgs = it->second;
    if (0 < msgs.size()) { // We return the first element
      return (MsgPreCommitFree)*(msgs.begin());
    }
  }
  View v = 0;
  MsgPreCommitFree msg(view,{});
  return msg;
}
