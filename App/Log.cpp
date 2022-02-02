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
