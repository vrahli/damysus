#ifndef MSG_H
#define MSG_H

#include <set>

#include "Nodes.h"
#include "Signs.h"
#include "RData.h"
#include "Proposal.h"
#include "Transaction.h"
#include "CData.h"
#include "Void.h"
#include "Cert.h"
#include "Accum.h"
#include "JBlock.h"
#include "CBlock.h"


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
  void serialize(salticidae::DataStream &s) const { s << trans << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << cid << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << reply; }
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
  void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << prop << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << rdata << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << cdata << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << cdata << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << cdata << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << cdata << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << data << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << acc << block << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << data << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << data << signs; }
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
  void serialize(salticidae::DataStream &s) const { s << data << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << block << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << data << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << data << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << block << sign; }
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
  void serialize(salticidae::DataStream &s) const { s << data << sign; }
};


#endif
