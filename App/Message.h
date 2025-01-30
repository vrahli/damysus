#ifndef MSG_H
#define MSG_H

#include <set>

#include "Nodes.h"
#include "Signs.h"
#include "Auth.h"
#include "Auths.h"
#include "RData.h"
#include "FData.h"
#include "Proposal.h"
#include "Transaction.h"
#include "CData.h"
#include "Void.h"
#include "Cert.h"
#include "Accum.h"
#include "HAccum.h"
#include "JBlock.h"
#include "CBlock.h"
#include "PJust.h"
#include "OPproposal.h"
#include "OPprepare.h"
#include "OPstore.h"
#include "OPstoreCert.h"
#include "OPvote.h"
#include "OPaccum.h"


#include "salticidae/msg.h"
#include "salticidae/stream.h"



/////////////////////////////////////////////////////
// Client messages


struct MsgTransaction {
  static const uint8_t opcode = HDR_TRANSACTION;
  salticidae::DataStream serialized;
  Transaction trans;
  Sign sign;
  MsgTransaction(const Transaction &trans, const Sign &sign) : trans(trans),sign(sign) { serialized << trans << sign; }
  MsgTransaction(salticidae::DataStream &&s) { s >> trans >> sign; }
  bool operator<(const MsgTransaction& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "TRANSACTION[" + trans.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(Transaction) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << trans << sign; }
};

struct MsgStart {
  static const uint8_t opcode = HDR_START;
  salticidae::DataStream serialized;
  CID cid;
  Sign sign;
  MsgStart(const CID &cid, const Sign &sign) : cid(cid),sign(sign) { serialized << cid << sign; }
  MsgStart(salticidae::DataStream &&s) { s >> cid >> sign; }
  bool operator<(const MsgStart& s) const {
    if (cid < s.cid) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "START[" + std::to_string(cid) + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(CID) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << cid << sign; }
};

/*struct MsgStop {
  static const uint8_t opcode = HDR_STOP;
  salticidae::DataStream serialized;
  CID cid;
  Sign sign;
  MsgStop(const CID &cid, const Sign &sign) : cid(cid),sign(sign) { serialized << cid << sign; }
  MsgStop(salticidae::DataStream &&s) { s >> cid >> sign; }
  bool operator<(const MsgStop& s) const {
    if (cid < s.cid) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "STOP[" + std::to_string(cid) + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(CID) + sizeof(Sign)); }
  void serialize(salticidae::DataStream &s) const { s << cid << sign; }
};*/

struct MsgReply {
  static const uint8_t opcode = HDR_REPLY;
  salticidae::DataStream serialized;
  unsigned int reply;
  MsgReply(const unsigned int &reply) : reply(reply) { serialized << reply; }
  MsgReply(salticidae::DataStream &&s) { s >> reply; }
  bool operator<(const MsgReply& s) const {
    if (reply < s.reply) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "REPLY[" + std::to_string(reply) + "]";
  }
  unsigned int sizeMsg() { return (sizeof(unsigned int)); }
  //void serialize(salticidae::DataStream &s) const { s << reply; }
};



/////////////////////////////////////////////////////
// Basic version - Baseline and Cheap


// TODO: replace Signs by Sign
struct MsgNewView {
  static const uint8_t opcode = HDR_NEWVIEW;
  salticidae::DataStream serialized;
  RData rdata;
  Signs signs;
  MsgNewView(const RData &rdata, const Signs &signs) : rdata(rdata),signs(signs) { serialized << rdata << signs; }
  MsgNewView(salticidae::DataStream &&s) { s >> rdata >> signs; }
  bool operator<(const MsgNewView& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "NEWVIEW[" + rdata.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
};

// TODO: replace Signs by Sign
struct MsgLdrPrepare {
  static const uint8_t opcode = HDR_PREPARE_LDR;
  salticidae::DataStream serialized;
  Proposal prop;
  Signs signs;
  MsgLdrPrepare() {}
  MsgLdrPrepare(const Proposal &prop, const Signs &signs) : prop(prop),signs(signs) { serialized << prop << signs; }
  MsgLdrPrepare(salticidae::DataStream &&s) { s >> prop >> signs; }
  bool operator<(const MsgLdrPrepare& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "LDRPREPARE[" + prop.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(Proposal) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << prop << signs; }
};

struct MsgPrepare {
  static const uint8_t opcode = HDR_PREPARE;
  salticidae::DataStream serialized;
  RData rdata;
  Signs signs;
  MsgPrepare(const RData &rdata, const Signs &signs) : rdata(rdata),signs(signs) { serialized << rdata << signs; }
  MsgPrepare(salticidae::DataStream &&s) { s >> rdata >> signs; }
  bool operator<(const MsgPrepare& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE[" + rdata.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
};

struct MsgPreCommit {
  static const uint8_t opcode = HDR_PRECOMMIT;
  salticidae::DataStream serialized;
  RData rdata;
  Signs signs;
  MsgPreCommit(const RData &rdata, const Signs &signs) : rdata(rdata),signs(signs) { serialized << rdata << signs; }
  MsgPreCommit(salticidae::DataStream &&s) { s >> rdata >> signs; }
  bool operator<(const MsgPreCommit& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PRECOMMIT[" + rdata.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
};

struct MsgCommit {
  static const uint8_t opcode = HDR_COMMIT;
  salticidae::DataStream serialized;
  RData rdata;
  Signs signs;
  MsgCommit(const RData &rdata, const Signs &signs) : rdata(rdata),signs(signs) { serialized << rdata << signs; }
  MsgCommit(salticidae::DataStream &&s) { s >> rdata >> signs; }
  bool operator<(const MsgCommit& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "COMMIT[" + rdata.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
};



/////////////////////////////////////////////////////
// Basic version - Quick


struct MsgNewViewAcc {
  static const uint8_t opcode = HDR_NEWVIEW_ACCUM;
  salticidae::DataStream serialized;
  CData<Void,Cert> cdata;
  Sign sign;
  MsgNewViewAcc(const CData<Void,Cert> &cdata, const Sign &sign) : cdata(cdata),sign(sign) { serialized << cdata << sign; }
  MsgNewViewAcc(salticidae::DataStream &&s) { s >> cdata >> sign; }
  bool operator<(const MsgNewViewAcc& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "NEWVIEW-ACCUM[" + cdata.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(CData<Void,Cert>) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << cdata << sign; }
};

struct MsgLdrPrepareAcc {
  static const uint8_t opcode = HDR_PREPARE_LDR_ACCUM;
  salticidae::DataStream serialized;
  CData<Block,Accum> cdata;
  Sign sign;
  MsgLdrPrepareAcc(const CData<Block,Accum> &cdata, const Sign &sign) : cdata(cdata),sign(sign) { serialized << cdata << sign; }
  MsgLdrPrepareAcc(salticidae::DataStream &&s) { s >> cdata >> sign; }
  bool operator<(const MsgLdrPrepareAcc& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE-LDR-ACCUM[" + cdata.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(CData<Block,Accum>) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << cdata << sign; }
};

struct MsgPrepareAcc {
  static const uint8_t opcode = HDR_PREPARE_ACCUM;
  salticidae::DataStream serialized;
  CData<Hash,Void> cdata;
  Signs signs;
  MsgPrepareAcc(const CData<Hash,Void> &cdata, const Signs &signs) : cdata(cdata),signs(signs) { serialized << cdata << signs; }
  MsgPrepareAcc(salticidae::DataStream &&s) { s >> cdata >> signs; }
  bool operator<(const MsgPrepareAcc& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE-ACCUM[" + cdata.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(CData<Hash,Void>) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << cdata << signs; }
};

struct MsgPreCommitAcc {
  static const uint8_t opcode = HDR_PRECOMMIT_ACCUM;
  salticidae::DataStream serialized;
  CData<Hash,Void> cdata;
  Signs signs;
  MsgPreCommitAcc(const CData<Hash,Void> &cdata, const Signs &signs) : cdata(cdata),signs(signs) { serialized << cdata << signs; }
  MsgPreCommitAcc(salticidae::DataStream &&s) { s >> cdata >> signs; }
  bool operator<(const MsgPreCommitAcc& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PRECOMMIT-ACCUM[" + cdata.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(CData<Hash,Void>) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << cdata << signs; }
};



/////////////////////////////////////////////////////
// Basic version - Cheap&Quick


struct MsgNewViewComb {
  static const uint8_t opcode = HDR_NEWVIEW_COMB;
  salticidae::DataStream serialized;
  RData data;
  Sign sign;
  MsgNewViewComb(const RData &data, const Sign &sign) : data(data),sign(sign) { serialized << data << sign; }
  MsgNewViewComb(salticidae::DataStream &&s) { s >> data >> sign; }
  bool operator<(const MsgNewViewComb& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "NEWVIEW-COMB[" + data.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << data << sign; }
};

struct MsgLdrPrepareComb {
  static const uint8_t opcode = HDR_PREPARE_LDR_COMB;
  salticidae::DataStream serialized;
  Accum acc;
  Block block;
  Sign sign;
  MsgLdrPrepareComb(const Accum &acc, const Block &block, const Sign &sign) : acc(acc),block(block),sign(sign) { serialized << acc << block << sign; }
  MsgLdrPrepareComb(salticidae::DataStream &&s) { s >> acc >> block >> sign; }
  bool operator<(const MsgLdrPrepareComb& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE-LDR-COMB[" + acc.prettyPrint() + "," + block.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(Accum) + sizeof(Block) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << acc << block << sign; }
};

struct MsgPrepareComb {
  static const uint8_t opcode = HDR_PREPARE_COMB;
  salticidae::DataStream serialized;
  RData data;
  Signs signs;
  MsgPrepareComb(const RData &data, const Signs &signs) : data(data),signs(signs) { serialized << data << signs; }
  MsgPrepareComb(salticidae::DataStream &&s) { s >> data >> signs; }
  bool operator<(const MsgPrepareComb& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE-COMB[" + data.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << data << signs; }
};

struct MsgPreCommitComb {
  static const uint8_t opcode = HDR_PRECOMMIT_COMB;
  salticidae::DataStream serialized;
  RData data;
  Signs signs;
  MsgPreCommitComb(const RData &data, const Signs &signs) : data(data),signs(signs) { serialized << data << signs; }
  MsgPreCommitComb(salticidae::DataStream &&s) { s >> data >> signs; }
  bool operator<(const MsgPreCommitComb& s) const {
    if (signs < s.signs) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PRECOMMIT-COMB[" + data.prettyPrint() + "," + signs.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Signs)); }
  //void serialize(salticidae::DataStream &s) const { s << data << signs; }
};




/////////////////////////////////////////////////////
// Basic version - FREE


struct MsgNewViewFree {
  static const uint8_t opcode = HDR_NEWVIEW_FREE;
  salticidae::DataStream serialized;
  FData data;
  Auth auth;
  MsgNewViewFree(const FData &data, const Auth &auth) : data(data),auth(auth) { serialized << data << auth; }
  MsgNewViewFree(salticidae::DataStream &&s) { s >> data >> auth; }
  bool operator<(const MsgNewViewFree& s) const {
    if (auth < s.auth) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "NEWVIEW-FREE[" + data.prettyPrint() + "," + auth.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(FData) + sizeof(Auth)); }
  //void serialize(salticidae::DataStream &s) const { s << data << auth; }
};


struct MsgLdrPrepareFree {
  static const uint8_t opcode = HDR_PREPARE_LDR_FREE;
  salticidae::DataStream serialized;
  HAccum acc;
  Block block;
  //MsgLdrPrepareFree() : acc(HAccum()),block(Block()) { serialized << acc << block; }
  //MsgLdrPrepareFree(const MsgLdrPrepareFree& m) : acc(m.acc),block(m.block) { serialized << acc << block; }
  MsgLdrPrepareFree(const HAccum &acc, const Block &block) : acc(acc),block(block) { serialized << acc << block; }
  MsgLdrPrepareFree(salticidae::DataStream &&s) { s >> acc >> block; }
  bool operator<(const MsgLdrPrepareFree& s) const {
    if (acc < s.acc) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE-LDR-FREE[" + acc.prettyPrint() + "," + block.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(HAccum) + sizeof(Block)); }
  //void serialize(salticidae::DataStream &s) const { s << acc << block; }
};

struct MsgBckPrepareFree {
  static const uint8_t opcode = HDR_PREPARE_BCK_FREE;
  salticidae::DataStream serialized;
  View view;
  Auth auth;
  MsgBckPrepareFree(const View &view, const Auth &auth) : view(view),auth(auth) { serialized << view << auth; }
  MsgBckPrepareFree(salticidae::DataStream &&s) { s >> view >> auth; }
  bool operator<(const MsgBckPrepareFree& s) const {
    if (auth < s.auth) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE-BCK-FREE[" + std::to_string(view) + "," + auth.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(View) + sizeof(Auth)); }
  //void serialize(salticidae::DataStream &s) const { s << view << auth; }
};

struct MsgPrepareFree {
  static const uint8_t opcode = HDR_PREPARE_FREE;
  salticidae::DataStream serialized;
  PJust just;
  MsgPrepareFree() : just(PJust(false)) { serialized << just; }
  MsgPrepareFree(const MsgPrepareFree& m) : just(m.just) { serialized << just; }
  MsgPrepareFree(const PJust &just) : just(just) { serialized << just; }
  MsgPrepareFree(salticidae::DataStream &&s) { s >> just; }
  bool operator<(const MsgPrepareFree& s) const {
    if (just < s.just) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE-FREE[" + just.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(PJust)); }
  unsigned int sizeAuth() { return just.sizeAuth(); }
  /*  void addAuth(Auth a) {
    just.add(a);
    serialized << just;
    }*/
  //void set(Hash h, View v, Auth a) { hash = h; view = v; auth = a; }
  //void serialize(salticidae::DataStream &s) const { s << just; }
};

struct MsgPreCommitFree {
  static const uint8_t opcode = HDR_PRECOMMIT_FREE;
  salticidae::DataStream serialized;
  View view;
  Auths auths;
  MsgPreCommitFree(const View &view, const Auths &auths) : view(view),auths(auths) { serialized << view << auths; }
  MsgPreCommitFree(salticidae::DataStream &&s) { s >> view >> auths; }
  bool operator<(const MsgPreCommitFree& s) const {
    if (auths < s.auths) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PRECOMMIT-FREE[" + std::to_string(view) + "," + auths.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(View) + sizeof(Auths)); }
  //void serialize(salticidae::DataStream &s) const { s << view << auths; }
};


/////////////////////////////////////////////////////
// Basic version - OP


struct OPcert {
  OPCtag tag;
  union{ OPprepare prep; OPvote vote; };
  // Constructors
  OPcert()            : tag(OPCprep),prep(OPprepare()) {}
  OPcert(OPprepare p) : tag(OPCprep),prep(p)           {}
  OPcert(OPvote v)    : tag(OPCvote),vote(v)           {}
  // Printing
  std::string toString() {
    if (tag == OPCprep) { return prep.toString(); }
    else return vote.toString();
  }
  std::string prettyPrint() {
    if (tag == OPCprep) { return prep.prettyPrint(); }
    else return vote.prettyPrint();
  }
  std::string data() {
    if (tag == OPCprep) { return prep.data(); }
    else return vote.data();
  }
  // Getters
  Hash getHash() {
    if (tag == OPCprep) { return prep.getHash(); }
    else return vote.getHash();
  }
  View getView() {
    if (tag == OPCprep) { return prep.getView(); }
    else return vote.getView();
  }
  Auths getAuths() {
    if (tag == OPCprep) { return prep.getAuths(); }
    else return vote.getAuths();
  }
  // Setters
  void setPrep(OPprepare p) {
    tag = OPCprep;
    prep = p;
  }
  void setVote(OPvote v) {
    tag = OPCvote;
    vote = v;
  }
  // Serialization
  void serialize(salticidae::DataStream &data) const {
    data << tag;
    if (tag == OPCprep) { data << prep; }
    else data << vote;
  }
  void unserialize(salticidae::DataStream &data) {
    data >> tag;
    if (tag == OPCprep) { data >> prep; }
    else data >> vote;
  }
};


struct OPnvcert {
  OPstore store;
  OPcert cert;
  // Constructors
  OPnvcert() : store(OPstore()),cert(OPcert()) {}
  OPnvcert(OPstore store, OPcert cert) : store(store),cert(cert) {}
  std::string prettyPrint() {
    return "NVC[" + store.prettyPrint() + "," + cert.prettyPrint() + "]";
  }
  std::string toString() {
    return (store.toString() + cert.toString());
  }
  void serialize(salticidae::DataStream &s) const { s << store << cert; }
  void unserialize(salticidae::DataStream &data) { data >> store >> cert; }
  bool operator<(const OPnvcert& s) const { return store < s.store; }
};


struct OPnvblock {
  Block block;
  OPnvcert cert;
  // Constructors
  OPnvblock() : block(Block()),cert(OPnvcert()) {}
  OPnvblock(Block block, OPnvcert cert) : block(block),cert(cert) {}
  std::string prettyPrint() {
    return "NVB[" + block.prettyPrint() + "," + cert.prettyPrint() + "]";
  }
  std::string toString() {
    return (block.toString() + cert.toString());
  }
  void serialize(salticidae::DataStream &s) const { s << block << cert; }
  void unserialize(salticidae::DataStream &data) { data >> block >> cert; }
  bool operator<(const OPnvblock& s) const { return cert < s.cert; }
};


struct OPnvhash {
  Hash hash;
  OPnvcert cert;
  // Constructors
  OPnvhash() : hash(Hash()),cert(OPnvcert()) {}
  OPnvhash(Hash hash, OPnvcert cert) : hash(hash),cert(cert) {}
  std::string prettyPrint() {
    return "NVH[" + hash.prettyPrint() + "," + cert.prettyPrint() + "]";
  }
  std::string toString() {
    return (hash.toString() + cert.toString());
  }
  void serialize(salticidae::DataStream &s) const { s << hash << cert; }
  void unserialize(salticidae::DataStream &data) { data >> hash >> cert; }
  bool operator<(const OPnvhash& s) const { return cert < s.cert; }
};


struct OPnvcerts {
  std::set<OPnvcert> certs;
  // Constructors
  OPnvcerts() : certs({}) {}
  OPnvcerts(OPnvcert cert) : certs({cert}) {}
  std::string prettyPrint() {
    std::string s;
    for (std::set<OPnvcert>::iterator it = certs.begin(); it!=certs.end(); ++it) {
      OPnvcert c = (OPnvcert)*it;
      s += "," + c.prettyPrint();
    }
    return s;
  }
  void insert(OPnvcert cert) { certs.insert(cert); }
  std::string toString() {
    std::string s;
    for (std::set<OPnvcert>::iterator it = certs.begin(); it!=certs.end(); ++it) {
      OPnvcert c = (OPnvcert)*it;
      s += c.toString();
    }
    return s;
  }
};

struct OPnvblocks {
  OPnvblock nv;
  OPnvcerts certs; // all stores in certs are meant to have a getView() == nv.cert.store.getView()
  // Constructors
  OPnvblocks() : nv(OPnvblock()),certs(OPnvcerts()) {}
  OPnvblocks(OPnvblock nv) : nv(nv),certs(OPnvcerts()) {}
  void new_block(OPnvblock new_nv) { certs.insert(nv.cert); nv = new_nv; }
  void insert(OPnvcert cert) { certs.insert(cert); }
  std::string prettyPrint() {
    return "NVBS[" + nv.prettyPrint() + "," + certs.prettyPrint() + "]";
  }
  std::string toString() {
    return (nv.toString() + certs.toString());
  }
  bool operator<(const OPnvblocks& s) const { return nv < s.nv; }
};


/*
salticidae::DataStream& operator>>(salticidae::DataStream &&data, const OPnvblock& s) {
  data >> s.block >> s.store >> s.cert;
  return data;
};
salticidae::DataStream& operator<<(salticidae::DataStream &&data, const OPnvblock& s) {
  data << s.block << s.store << s.cert;
  return data;
};
*/


struct OPnv {
  OPNVtag tag;
  union{ OPprepare prep; OPnvblock nv; };
  // Constructors
  OPnv() : tag(OPNVa),prep(OPprepare()) {}
  OPnv(OPprepare prep) : tag(OPNVa),prep(prep) {}
  OPnv(OPnvblock nv) : tag(OPNVb),nv(nv) {}
  // Printing
  std::string toString() {
    if (tag == OPNVa) { return prep.toString(); }
    return nv.toString();
  }
  std::string prettyPrint() {
    if (tag == OPNVa) { return prep.prettyPrint(); }
    return nv.prettyPrint();
  }
  // Getters
  Hash getHash() {
    if (tag == OPNVa) { return prep.getHash(); }
    return nv.cert.store.getHash();
  }
  View getView() {
    if (tag == OPCprep) { return prep.getView(); }
    return nv.cert.store.getView();
  }
  // Setters
  void setPrep(OPprepare p) {
    tag = OPNVa;
    prep = p;
  }
  void setVote(OPnvblock n) {
    tag = OPNVb;
    nv = n;
  }
  bool operator<(const OPnv& s) const {
    if (tag == s.tag) {
      if (tag == OPNVa) { return prep < s.prep; }
      else { return nv < s.nv; }
    } else { return tag < s.tag; }
  }
/*  salticidae::DataStream& operator>>(salticidae::DataStream &data, const OPnv& s) {
    data >> s.tag;
    if (tag == OPNVa) { data >> s.prep; }
    else { data >> s.nv; }
    return data;
  }
  salticidae::DataStream& operator<<(salticidae::DataStream &data, const OPnv& s) {
    data << s.tag;
    if (tag == OPNVa) { data << s.prep; }
    else { data << s.nv; }
    return data;
  }*/
  // Serialization
  void serialize(salticidae::DataStream &data) const {
    data << tag;
    if (tag == OPNVa) { data << prep; }
    else { data << nv; }
  }
  void unserialize(salticidae::DataStream &data) {
    data >> tag;
    if (tag == OPNVa) { data >> prep; }
    else { data >> nv; }
  }
};


struct OPprp {
  Block block;
  OPproposal prop;
  OPcert cert;
  // Constructors
  OPprp() : block(Block()),prop(OPproposal()),cert(OPcert()) {}
  OPprp(Block block, OPproposal prop, OPcert cert) : block(block),prop(prop),cert(cert) {}
  std::string prettyPrint() {
    return "PRP[" + block.prettyPrint() + "," + prop.prettyPrint() + "," + cert.prettyPrint() + "]";
  }
  std::string toString() {
    return (block.toString() + prop.toString() + cert.toString());
  }
  void serialize(salticidae::DataStream &s) const { s << block << prop << cert; }
  void unserialize(salticidae::DataStream &data) { data >> block >> prop >> cert; }
  bool operator<(const OPprp& s) const { return (prop < s.prop); }
};


struct MsgNewViewOPA {
  static const uint8_t opcode = HDR_NEWVIEW_OPA;
  salticidae::DataStream serialized;
  OPprepare prep;
  MsgNewViewOPA(const OPprepare &prep) : prep(prep) { serialized << prep; }
  MsgNewViewOPA(salticidae::DataStream &&s) { s >> prep; }
  bool operator<(const MsgNewViewOPA& s) const { return (prep < s.prep); }
  std::string prettyPrint() { return "NEWVIEW-OPA[" + prep.prettyPrint() + "]"; }
  unsigned int sizeMsg() { return (sizeof(OPprepare)); }
  //void serialize(salticidae::DataStream &s) const { s << prep; }
};


struct MsgNewViewOPB {
  static const uint8_t opcode = HDR_NEWVIEW_OPB;
  salticidae::DataStream serialized;
  OPnvblock nv;
  MsgNewViewOPB(const OPnvblock &nv) : nv(nv) { serialized << nv; }
  MsgNewViewOPB(salticidae::DataStream &&s) { s >> nv; }
  bool operator<(const MsgNewViewOPB& s) const { return (nv < s.nv); }
  std::string prettyPrint() { return "NEWVIEW-OPB[" + nv.prettyPrint() + "]"; }
  unsigned int sizeMsg() { return (sizeof(OPnvblock)); }
  //void serialize(salticidae::DataStream &s) const { s << nv; }
};


// A special MsgNewViewOPB message for prepare certificates and without blocks
struct MsgNewViewOPBB {
  static const uint8_t opcode = HDR_NEWVIEW_OPBB;
  salticidae::DataStream serialized;
  OPstore store;
  OPprepare prep;
  MsgNewViewOPBB(const OPstore &store, const OPprepare &prep) : store(store),prep(prep) { serialized << store << prep; }
  MsgNewViewOPBB(salticidae::DataStream &&s) { s >> store >> prep; }
  bool operator<(const MsgNewViewOPBB& s) const { return (store < s.store); }
  std::string prettyPrint() { return "NEWVIEW-OPBB[" + store.prettyPrint() + "," + prep.prettyPrint() + "]"; }
  unsigned int sizeMsg() { return (sizeof(OPstore) + sizeof(OPprepare)); }
  //void serialize(salticidae::DataStream &s) const { s << nv; }
};


// These messages are sent by the leaders to the replicas in the prepare phase, in the "good" cases
struct MsgLdrPrepareOPA {
  static const uint8_t opcode = HDR_PREPARE_LDR_OPA;
  salticidae::DataStream serialized;
  Block block;
  OPproposal prop;
  OPprepare prep;
  // OK
  MsgLdrPrepareOPA() : block(Block()),prop(OPproposal()),prep(OPprepare()) { serialized << block << prop << prep; }
  MsgLdrPrepareOPA(const MsgLdrPrepareOPA& m) : block(m.block),prop(m.prop),prep(m.prep) { serialized << block << prop << prep; }
  MsgLdrPrepareOPA(const Block &block, const OPproposal &prop, const OPprepare &prep) : block(block),prop(prop),prep(prep) { serialized << block << prop << prep; }
  // OK
  //MsgLdrPrepareOPA(const Block &block, const OPproposal &prop, const OPprepare &prep, const bool &b) : block(block),prop(prop),prep(prep) { }
  // fails
  MsgLdrPrepareOPA(const Block &block, const OPproposal &prop, const OPprepare &prep, const bool &b) : block(block),prop(prop),prep(prep) { serialized << 0; }
  // fails
  MsgLdrPrepareOPA(const Block &block) : block(Block()),prop(OPproposal()),prep(OPprepare()) { serialized << block << prop << prep; }
  // fails
  MsgLdrPrepareOPA(unsigned int n) : block(Block()),prop(OPproposal()),prep(OPprepare()) { serialized << block << prop << prep; }
  MsgLdrPrepareOPA(salticidae::DataStream &&s) { s >> block >> prop >> prep; }
  bool operator<(const MsgLdrPrepareOPA& s) const { return (prop < s.prop); }
  std::string prettyPrint() {
    return "PREPARE-LDR-OP[" + block.prettyPrint() + "," + prop.prettyPrint() + "," + prep.prettyPrint() + "]";
  }
  std::string toString() { return block.toString() + prop.toString() + prep.toString(); }
  unsigned int sizeMsg() { return (sizeof(Block) + sizeof(OPproposal) + sizeof(OPprepare)); }
  //void serialize(salticidae::DataStream &s) const { s << block << prop << prep; }
};


// This is not used anymore
struct MsgLdrPrepareOPB {
  static const uint8_t opcode = HDR_PREPARE_LDR_OPB;
  salticidae::DataStream serialized;
  Block block;
  OPproposal prop;
  OPaccum acc;
  OPprepare prep;
  MsgLdrPrepareOPB() : block(Block()),prop(OPproposal()),acc(OPaccum()),prep(OPprepare()) { serialized << block << prop << acc << prep; }
  MsgLdrPrepareOPB(const MsgLdrPrepareOPB& m) : block(m.block),prop(m.prop),acc(m.acc),prep(m.prep) { serialized << block << prop << acc << prep; }
  MsgLdrPrepareOPB(const Block &block, const OPproposal &prop, const OPaccum &acc, const OPprepare &prep) : block(block),prop(prop),acc(acc),prep(prep) { serialized << block << prop << acc << prep; }
  MsgLdrPrepareOPB(salticidae::DataStream &&s) { s >> block >> prop >> acc >> prep; }
  bool operator<(const MsgLdrPrepareOPB& s) const { return (prop < s.prop); }
  std::string prettyPrint() {
    return "PREPARE-LDR-OPB[" + block.prettyPrint() + "," + prop.prettyPrint() + "," + acc.prettyPrint() + "," + prep.prettyPrint() + "]";
  }
  std::string toString() { return block.toString() + prop.toString() + acc.toString() + prep.toString(); }
  unsigned int sizeMsg() { return (sizeof(Block) + sizeof(OPproposal) + sizeof(OPaccum) + sizeof(OPprepare)); }
  //void serialize(salticidae::DataStream &s) const { s << block << prop << acc << prep; }
};


// These messages are sent by the leaders to the replicas at the end of the additional phase
struct MsgLdrPrepareOPC {
  static const uint8_t opcode = HDR_PREPARE_LDR_OPC;
  salticidae::DataStream serialized;
  Block block;
  OPproposal prop;
  OPvote vote;
  MsgLdrPrepareOPC() : block(Block()),prop(OPproposal()),vote(OPvote()) { serialized << block << prop << vote; }
  MsgLdrPrepareOPC(const MsgLdrPrepareOPC& m) : block(m.block),prop(m.prop),vote(m.vote) { serialized << block << prop << vote; }
  MsgLdrPrepareOPC(const Block &block, const OPproposal &prop, const OPvote &vote) : block(block),prop(prop),vote(vote) { serialized << block << prop << vote; }
  MsgLdrPrepareOPC(salticidae::DataStream &&s) { s >> block >> prop >> vote; }
  bool operator<(const MsgLdrPrepareOPC& s) const { return (prop < s.prop); }
  std::string prettyPrint() {
    return "PREPARE-LDR-OPC[" + block.prettyPrint() + "," + prop.prettyPrint() + "," + vote.prettyPrint() + "]";
  }
  std::string toString() { return block.toString() + prop.toString() + vote.toString(); }
  unsigned int sizeMsg() { return (sizeof(Block) + sizeof(OPproposal) + sizeof(OPvote)); }
  //void serialize(salticidae::DataStream &s) const { s << block << prop << vote; }
};


struct LdrPrepareOP {
  Block block;
  OPproposal prop;
  OPCtag tag;
  OPaccum acc;
  OPprepare prep;
  OPvote vote;
  // Constructors
  LdrPrepareOP() : block(Block()),prop(OPproposal()),tag(OPCprep),acc(OPaccum()),prep(OPprepare()),vote(OPvote()) {}
  LdrPrepareOP(const MsgLdrPrepareOPA &p) : block(p.block),prop(p.prop),tag(OPCprep),acc(OPaccum()),prep(p.prep),vote(OPvote()) {}
  LdrPrepareOP(const MsgLdrPrepareOPB &p) : block(p.block),prop(p.prop),tag(OPCacc),acc(p.acc),prep(p.prep),vote(OPvote()) {}
  LdrPrepareOP(const MsgLdrPrepareOPC &p) : block(p.block),prop(p.prop),tag(OPCvote),acc(OPaccum()),prep(OPprepare()),vote(p.vote) {}
  // Printing
  std::string prettyPrint() {
    return "LDRPREPARE[" + block.prettyPrint() + "," + prop.prettyPrint() + "," + std::to_string(tag) + "," + acc.prettyPrint() + "," + prep.prettyPrint() + "," + vote.prettyPrint() + "]";
  }
  std::string toString() { return block.toString() + prop.toString() + std::to_string(tag) + acc.toString() + prep.toString() + vote.toString(); }
};


struct MsgBckPrepareOP {
  static const uint8_t opcode = HDR_PREPARE_BCK_OP;
  salticidae::DataStream serialized;
  OPstore store;
  MsgBckPrepareOP() : store(OPstore()) { serialized << store; }
  MsgBckPrepareOP(const MsgBckPrepareOP& m) : store(m.store) { serialized << store; }
  MsgBckPrepareOP(const OPstore &store) : store(store) { serialized << store; }
  MsgBckPrepareOP(salticidae::DataStream &&s) { s >> store; }
  bool operator<(const MsgBckPrepareOP& s) const { return (store < s.store); }
  std::string prettyPrint() {
    return "PREPARE-BCK-OP[" + store.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(OPstore)); }
  //void serialize(salticidae::DataStream &s) const { s << store; }
};


struct MsgPreCommitOP {
  static const uint8_t opcode = HDR_PRECOMMIT_OP;
  salticidae::DataStream serialized;
  OPprepare cert;
  MsgPreCommitOP(const OPprepare &cert) : cert(cert) { serialized << cert; }
  MsgPreCommitOP(salticidae::DataStream &&s) { s >> cert; }
  bool operator<(const MsgPreCommitOP& s) const { return (cert < s.cert); }
  std::string prettyPrint() { return "PRECOMMIT-OP[" + cert.prettyPrint() + "]"; }
  unsigned int sizeMsg() { return (sizeof(OPprepare)); }
  //void serialize(salticidae::DataStream &s) const { s << cert; }
};


struct MsgLdrAddOP {
  static const uint8_t opcode = HDR_ADD_LDR_OP;
  salticidae::DataStream serialized;
  OPaccum acc;
  OPnvblock nv;
  MsgLdrAddOP() : acc(OPaccum()),nv(OPnvblock()) { serialized << acc << nv; }
  MsgLdrAddOP(const MsgLdrAddOP& m) : acc(m.acc),nv(m.nv) { serialized << acc << nv; }
  MsgLdrAddOP(const OPaccum &acc, const OPnvblock &nv) : acc(acc),nv(nv) { serialized << acc << nv; }
  MsgLdrAddOP(salticidae::DataStream &&s) { s >> acc >> nv; }
  bool operator<(const MsgLdrAddOP& s) const { return (acc < s.acc); }
  std::string prettyPrint() { return "ADD-LDR-OP[" + acc.prettyPrint() + "," + nv.prettyPrint() + "]"; }
  std::string toString() { return acc.toString() + nv.toString(); }
  unsigned int sizeMsg() { return (sizeof(OPaccum) + sizeof(OPnvblock)); }
  //void serialize(salticidae::DataStream &s) const { s << acc << nv; }
};


struct MsgBckAddOP {
  static const uint8_t opcode = HDR_ADD_BCK_OP;
  salticidae::DataStream serialized;
  OPvote vote;
  MsgBckAddOP() : vote(OPvote()) { serialized << vote; }
  MsgBckAddOP(const MsgBckAddOP& m) : vote(m.vote) { serialized << vote; }
  MsgBckAddOP(const OPvote &vote) : vote(vote) { serialized << vote; }
  MsgBckAddOP(salticidae::DataStream &&s) { s >> vote; }
  bool operator<(const MsgBckAddOP& s) const { return (vote < s.vote); }
  std::string prettyPrint() { return "ADD-BCK-OP[" + vote.prettyPrint() + "]"; }
  std::string toString() { return vote.toString(); }
  unsigned int sizeMsg() { return (sizeof(OPvote)); }
  //void serialize(salticidae::DataStream &s) const { s << vote; }
};


/////////////////////////////////////////////////////
// Chained Version - Baseline


struct MsgNewViewCh {
  static const uint8_t opcode = HDR_NEWVIEW_CH;
  salticidae::DataStream serialized;
  RData data;
  Sign sign;
  MsgNewViewCh(const RData &data, const Sign &sign) : data(data),sign(sign) { serialized << data << sign; }
  MsgNewViewCh(salticidae::DataStream &&s) { s >> data >> sign; }
  bool operator<(const MsgNewViewCh& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "NEWVIEW[" + data.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << data << sign; }
};


struct MsgLdrPrepareCh {
  static const uint8_t opcode = HDR_PREPARE_LDR_CH;
  salticidae::DataStream serialized;
  JBlock block;
  Sign sign;
  MsgLdrPrepareCh() {}
  MsgLdrPrepareCh(const JBlock &block, const Sign &sign) : block(block),sign(sign) { serialized << block << sign; }
  MsgLdrPrepareCh(salticidae::DataStream &&s) { s >> block >> sign; }
  bool operator<(const MsgLdrPrepareCh& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "LDRPREPARE[" + block.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(JBlock) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << block << sign; }
};


struct MsgPrepareCh {
  static const uint8_t opcode = HDR_PREPARE_CH;
  salticidae::DataStream serialized;
  RData data;
  Sign sign;
  MsgPrepareCh(const RData &data, const Sign &sign) : data(data),sign(sign) { serialized << data << sign; }
  MsgPrepareCh(salticidae::DataStream &&s) { s >> data >> sign; }
  bool operator<(const MsgPrepareCh& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE[" + data.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << data << sign; }
};



/////////////////////////////////////////////////////
// Chained Version - Cheap&Quick


struct MsgNewViewChComb {
  static const uint8_t opcode = HDR_NEWVIEW_CH_COMB;
  salticidae::DataStream serialized;
  RData data;
  Sign sign;
  MsgNewViewChComb(const RData &data, const Sign &sign) : data(data),sign(sign) { serialized << data << sign; }
  MsgNewViewChComb(salticidae::DataStream &&s) { s >> data >> sign; }
  bool operator<(const MsgNewViewChComb& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "NEWVIEW[" + data.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << data << sign; }
};


struct MsgLdrPrepareChComb {
  static const uint8_t opcode = HDR_PREPARE_LDR_CH_COMB;
  salticidae::DataStream serialized;
  CBlock block;
  Sign sign;
  MsgLdrPrepareChComb() {}
  MsgLdrPrepareChComb(const CBlock &block, const Sign &sign) : block(block),sign(sign) { serialized << block << sign; }
  MsgLdrPrepareChComb(salticidae::DataStream &&s) { s >> block >> sign; }
  bool operator<(const MsgLdrPrepareChComb& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "LDRPREPARE[" + block.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(CBlock) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << block << sign; }
};


struct MsgPrepareChComb {
  static const uint8_t opcode = HDR_PREPARE_CH_COMB;
  salticidae::DataStream serialized;
  RData data;
  Sign sign;
  MsgPrepareChComb(const RData &data, const Sign &sign) : data(data),sign(sign) { serialized << data << sign; }
  MsgPrepareChComb(salticidae::DataStream &&s) { s >> data >> sign; }
  bool operator<(const MsgPrepareChComb& s) const {
    if (sign < s.sign) { return true; }
    return false;
  }
  std::string prettyPrint() {
    return "PREPARE[" + data.prettyPrint() + "," + sign.prettyPrint() + "]";
  }
  unsigned int sizeMsg() { return (sizeof(RData) + sizeof(Sign)); }
  //void serialize(salticidae::DataStream &s) const { s << data << sign; }
};


#endif
