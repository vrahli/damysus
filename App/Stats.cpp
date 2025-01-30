#include <iostream>
#include <sstream>

#include "Stats.h"


Stats::Stats() {
  this->id         = 0;

  this->TEEverify  = 0.0;
  this->TEEsign    = 0.0;
  this->TEEprepare = 0.0;
  this->TEEstore   = 0.0;
  this->TEEaccum   = 0.0;
  this->TEEtime    = 0.0;

  this->cryptoSignNum  = 0;
  this->cryptoVerifNum = 0;
  this->cryptoSignTime  = 0.0;
  this->cryptoVerifTime = 0.0;

  this->totalHandleTime = 0.0;
  this->totalViewTime   = 0.0;
  this->totalNvTime     = 0.0;
  this->totalLPrepTime  = 0.0;
  this->totalLPrepbTime = 0.0;
  this->totalBPrepaTime = 0.0;
  this->totalBPrepbTime = 0.0;
  this->totalBPrepcTime = 0.0;
  this->totalLVoteTime  = 0.0;
  this->totalBVoteTime  = 0.0;
  this->totalLAddTime   = 0.0;
  this->totalBAddTime   = 0.0;
  this->totalDecTime    = 0.0;
  this->totalReplyTime  = 0.0;
  this->totalStartTime  = 0.0;
  this->totalSendTime   = 0.0;
  this->totalEarlyTime  = 0.0;
  this->totalGen0Time   = 0.0;
  this->totalGen1Time   = 0.0;
  this->totalGen2Time   = 0.0;
  this->totalNewTime    = 0.0;
  this->totalPCommTime  = 0.0;

  this->timeouts  = 0;
  this->numonepbs = 0;
  this->numonepcs = 0;
}

void Stats::setId(PID i) { this->id = i; }
PID Stats::getId() { return this->id; }

void Stats::addTEEverify(double v)  { this->TEEverify  += v; }
void Stats::addTEEsign(double v)    { this->TEEsign    += v; }
void Stats::addTEEprepare(double v) { this->TEEprepare += v; }
void Stats::addTEEstore(double v)   { this->TEEstore   += v; }
void Stats::addTEEaccum(double v)   { this->TEEaccum   += v; }
void Stats::addTEEtime(double v)    { this->TEEtime    += v; }

void Stats::addCryptoSignTime(double v)  { this->cryptoSignNum  += 1; this->cryptoSignTime  += v; }
void Stats::addCryptoVerifTime(double v) { this->cryptoVerifNum += 1; this->cryptoVerifTime += v; }

void Stats::addTotalHandleTime(double v) { this->totalHandleTime += v; this->handleTimes.push_back(v); }
void Stats::addTotalViewTime(double v)   { this->totalViewTime   += v; this->viewTimes.push_back(v);   }
void Stats::addTotalLPrepTime(double v)  { this->totalLPrepTime  += v; this->lprepTimes.push_back(v);  }
void Stats::addTotalLPrepbTime(double v) { this->totalLPrepbTime += v; this->lprepbTimes.push_back(v); }
void Stats::addTotalBPrepaTime(double v) { this->totalBPrepaTime += v; this->bprepaTimes.push_back(v); }
void Stats::addTotalBPrepbTime(double v) { this->totalBPrepbTime += v; this->bprepbTimes.push_back(v); }
void Stats::addTotalBPrepcTime(double v) { this->totalBPrepcTime += v; this->bprepcTimes.push_back(v); }
void Stats::addTotalLVoteTime(double v)  { this->totalLVoteTime  += v; this->lvoteTimes.push_back(v);  }
void Stats::addTotalBVoteTime(double v)  { this->totalBVoteTime  += v; this->bvoteTimes.push_back(v);  }
void Stats::addTotalNvTime(double v)     { this->totalNvTime     += v; this->nvTimes.push_back(v);     }
void Stats::addTotalLAddTime(double v)   { this->totalLAddTime   += v; this->laddTimes.push_back(v);   }
void Stats::addTotalBAddTime(double v)   { this->totalBAddTime   += v; this->baddTimes.push_back(v);   }
void Stats::addTotalDecTime(double v)    { this->totalDecTime    += v; this->decTimes.push_back(v);    }
void Stats::addTotalReplyTime(double v)  { this->totalReplyTime  += v; this->replyTimes.push_back(v);  }
void Stats::addTotalStartTime(double v)  { this->totalStartTime  += v; this->startTimes.push_back(v);  }
void Stats::addTotalSendTime(double v)   { this->totalSendTime   += v; this->sendTimes.push_back(v);   }
void Stats::addTotalEarlyTime(double v)  { this->totalEarlyTime  += v; this->earlyTimes.push_back(v);  }
void Stats::addTotalGen0Time(double v)   { this->totalGen0Time   += v; this->gen0Times.push_back(v);   }
void Stats::addTotalGen1Time(double v)   { this->totalGen1Time   += v; this->gen1Times.push_back(v);   }
void Stats::addTotalGen2Time(double v)   { this->totalGen2Time   += v; this->gen2Times.push_back(v);   }
void Stats::addTotalNewTime(double v)    { this->totalNewTime    += v; this->newTimes.push_back(v);    }
void Stats::addTotalPCommTime(double v)  { this->totalPCommTime  += v; this->pcommTimes.push_back(v);  }

unsigned int Stats::getCryptoSignNum()  { return this->cryptoSignNum; }
unsigned int Stats::getCryptoVerifNum() { return this->cryptoVerifNum; }

double Stats::getCryptoSignTime()  { return this->cryptoSignTime; }
double Stats::getCryptoVerifTime() { return this->cryptoVerifTime; }

Times getTotalTime(unsigned int quant, std::vector<double> l) {
  unsigned int k = 0; // number of values to remove from both sides
  if (quant > 0) { k = l.size() / (100 / quant); }
  for (int i = 0; i < k; i++) {
    l.erase(l.begin());
  }
  for (int i = 0; i < k; i++) {
    l.erase(--l.rbegin().base());
  }
  unsigned int tot = 0;
  unsigned int n = 0;
  for (std::vector<double>::iterator it=l.begin(); it!=l.end(); ++it) {
    tot += (double)*it;
    n++;
  }
  return Times(n,tot);
  //return this->totalHandleTime;
}

Times Stats::getTotalHandleTime(unsigned int quant) { return getTotalTime(quant, this->handleTimes); }
Times Stats::getTotalViewTime(unsigned int quant)   { return getTotalTime(quant, this->viewTimes);   }
Times Stats::getTotalLPrepTime(unsigned int quant)  { return getTotalTime(quant, this->lprepTimes);  }
Times Stats::getTotalLPrepbTime(unsigned int quant) { return getTotalTime(quant, this->lprepbTimes); }
Times Stats::getTotalBPrepaTime(unsigned int quant) { return getTotalTime(quant, this->bprepaTimes); }
Times Stats::getTotalBPrepbTime(unsigned int quant) { return getTotalTime(quant, this->bprepbTimes); }
Times Stats::getTotalBPrepcTime(unsigned int quant) { return getTotalTime(quant, this->bprepcTimes); }
Times Stats::getTotalLVoteTime(unsigned int quant)  { return getTotalTime(quant, this->lvoteTimes);  }
Times Stats::getTotalBVoteTime(unsigned int quant)  { return getTotalTime(quant, this->bvoteTimes);  }
Times Stats::getTotalNvTime(unsigned int quant)     { return getTotalTime(quant, this->nvTimes);     }
Times Stats::getTotalLAddTime(unsigned int quant)   { return getTotalTime(quant, this->laddTimes);   }
Times Stats::getTotalBAddTime(unsigned int quant)   { return getTotalTime(quant, this->baddTimes);   }
Times Stats::getTotalDecTime(unsigned int quant)    { return getTotalTime(quant, this->decTimes);    }
Times Stats::getTotalReplyTime(unsigned int quant)  { return getTotalTime(quant, this->replyTimes);  }
Times Stats::getTotalStartTime(unsigned int quant)  { return getTotalTime(quant, this->startTimes);  }
Times Stats::getTotalSendTime(unsigned int quant)   { return getTotalTime(quant, this->sendTimes);   }
Times Stats::getTotalEarlyTime(unsigned int quant)  { return getTotalTime(quant, this->earlyTimes);  }
Times Stats::getTotalGen0Time(unsigned int quant)   { return getTotalTime(quant, this->gen0Times);   }
Times Stats::getTotalGen1Time(unsigned int quant)   { return getTotalTime(quant, this->gen1Times);   }
Times Stats::getTotalGen2Time(unsigned int quant)   { return getTotalTime(quant, this->gen2Times);   }
Times Stats::getTotalNewTime(unsigned int quant)    { return getTotalTime(quant, this->newTimes);    }
Times Stats::getTotalPCommTime(unsigned int quant)  { return getTotalTime(quant, this->pcommTimes);  }

unsigned int Stats::getTimeouts()  { return this->timeouts;  }
unsigned int Stats::getNumOnePBs() { return this->numonepbs; }
unsigned int Stats::getNumOnePCs() { return this->numonepcs; }

void Stats::incExecViews() { this->execViews++; }
void Stats::incTimeouts()  { this->timeouts++;  }
void Stats::incNumOnePBs() { this->numonepbs++; }
void Stats::incNumOnePCs() { this->numonepcs++; }

void Stats::startExecTime(View v, Time t) {
  this->execTime[v] = std::make_tuple(false,t,t);
}

void Stats::endExecTime(View v, Time t) {
  std::map<View,std::tuple<bool,Time,Time>>::iterator it = this->execTime.find(v);
  if (it != this->execTime.end()) {
    std::tuple<bool,Time,Time> p = (std::tuple<bool,Time,Time>)it->second;
    this->execTime[v] = std::make_tuple(true, std::get<1>(p), t);
  }
}

double Stats::getExecTimeAvg() {
  unsigned int i = 0;
  double total = 0;
  for (std::map<View,std::tuple<bool,Time,Time>>::iterator it = this->execTime.begin(); it != this->execTime.end(); ++it) {
    std::tuple<bool,Time,Time> p = (std::tuple<bool,Time,Time>)it->second;
    if (std::get<0>(p)) {
      i++;
      total += std::chrono::duration_cast<std::chrono::microseconds>(std::get<2>(p) - std::get<1>(p)).count();
    }
  }
  return (total / i);
}


void printTimes(std::string tag, std::vector<double> l) {
  std::string s = "[" + tag;
  for (std::vector<double>::iterator it=l.begin(); it!=l.end(); ++it) {
    s += ";" + std::to_string((double)*it);
  }
  s += "]";
  std::cout << KMAG << s << KNRM << std::endl;
}

void Stats::printAllTimes() {
  printTimes("[id=" + std::to_string(this->id) + "-handle", this->handleTimes);
  printTimes("[id=" + std::to_string(this->id) + "-view",   this->viewTimes);
  printTimes("[id=" + std::to_string(this->id) + "-lprep",  this->lprepTimes);
  printTimes("[id=" + std::to_string(this->id) + "-lprepb", this->lprepbTimes);
  printTimes("[id=" + std::to_string(this->id) + "-bprepa", this->bprepaTimes);
  printTimes("[id=" + std::to_string(this->id) + "-bprepb", this->bprepbTimes);
  printTimes("[id=" + std::to_string(this->id) + "-bprepc", this->bprepcTimes);
  printTimes("[id=" + std::to_string(this->id) + "-lvote",  this->lvoteTimes);
  printTimes("[id=" + std::to_string(this->id) + "-bvote",  this->bvoteTimes);
  printTimes("[id=" + std::to_string(this->id) + "-ladd",   this->laddTimes);
  printTimes("[id=" + std::to_string(this->id) + "-badd",   this->baddTimes);
  printTimes("[id=" + std::to_string(this->id) + "-nv",     this->nvTimes);
  printTimes("[id=" + std::to_string(this->id) + "-dec",    this->decTimes);
  printTimes("[id=" + std::to_string(this->id) + "-reply",  this->replyTimes);
  printTimes("[id=" + std::to_string(this->id) + "-start",  this->startTimes);
  //printTimes("[id=" + std::to_string(this->id) + "-send",   this->sendTimes);
  printTimes("[id=" + std::to_string(this->id) + "-early",  this->earlyTimes);
  printTimes("[id=" + std::to_string(this->id) + "-gen0",   this->gen0Times);
  printTimes("[id=" + std::to_string(this->id) + "-gen1",   this->gen1Times);
  printTimes("[id=" + std::to_string(this->id) + "-gen2",   this->gen2Times);
  printTimes("[id=" + std::to_string(this->id) + "-new",    this->newTimes);
  printTimes("[id=" + std::to_string(this->id) + "-pcomm",  this->pcommTimes);
}

std::string Stats::toString() {
    std::ostringstream os;
    os << "[id="              << this->id
       << ";execViews="       << this->execViews
       << ";#timeouts="       << this->timeouts
       << ";#numonepbs="      << this->numonepbs
       << ";#numonepcs="      << this->numonepcs
       << ";TEEverify="       << this->TEEverify/this->execViews
       << ";TEEsign="         << this->TEEsign/this->execViews
       << ";TEEprepare="      << this->TEEprepare/this->execViews
       << ";TEEstore="        << this->TEEstore/this->execViews
       << ";TEEaccum="        << this->TEEaccum/this->execViews
       << ";TEEtime="         << this->TEEtime/this->execViews
//       << ";totalLPrepTime="  << this->totalLPrepTime/this->execViews
//       << ";totalBPrepTime="  << this->totalBPrepTime/this->execViews
//       << ";totalLVoteTime="  << this->totalLVoteTime/this->execViews
//       << ";totalBVoteTime="  << this->totalBVoteTime/this->execViews
//       << ";totalLAddTime="   << this->totalLAddTime/this->execViews
//       << ";totalBAddTime="   << this->totalBAddTime/this->execViews
//       << ";totalDecTime="    << this->totalDecTime/this->execViews
//       << ";totalNvTime="     << this->totalNvTime/this->execViews
       << ";cryptoSignNum="   << this->cryptoSignNum
       << ";cryptoVerifNum="  << this->cryptoVerifNum
       << ";cryptoSignTime="  << this->cryptoSignTime
       << ";cryptoVerifTime=" << this->cryptoVerifTime
       << ";totalHandleTime=" << this->totalHandleTime/this->execViews
       << ";totalViewTime="   << this->totalViewTime/this->execViews
       << "]";
    return os.str();
}

std::ostream& operator<<(std::ostream& os, const Stats& s) {
    os << std::to_string(s.id);
    return os;
}
