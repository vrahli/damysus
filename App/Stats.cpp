#include "Stats.h"
#include <sstream>

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
  this->totalPrepTime   = 0.0;
  this->totalNvTime     = 0.0;
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

void Stats::addTotalHandleTime(double v) {
  this->totalHandleTime += v;
  this->handleTimes.insert(v);
}
void Stats::addTotalViewTime(double v) {
  this->totalViewTime += v;
  this->viewTimes.insert(v);
}
void Stats::addTotalPrepTime(double v) {
  this->totalPrepTime += v;
  this->prepTimes.insert(v);
}
void Stats::addTotalNvTime(double v) {
  this->totalNvTime += v;
  this->nvTimes.insert(v);
}

unsigned int Stats::getCryptoSignNum()  { return this->cryptoSignNum; }
unsigned int Stats::getCryptoVerifNum() { return this->cryptoVerifNum; }

double Stats::getCryptoSignTime()  { return this->cryptoSignTime; }
double Stats::getCryptoVerifTime() { return this->cryptoVerifTime; }

Times Stats::getTotalHandleTime(unsigned int quant) {
  std::multiset<double> l = this->handleTimes;
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
  for (std::multiset<double>::iterator it=l.begin(); it!=l.end(); ++it) {
    tot += (double)*it;
    n++;
  }
  return Times(n,tot);
  //return this->totalHandleTime;
}

Times Stats::getTotalViewTime(unsigned int quant) {
  std::multiset<double> l = this->viewTimes;
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
  for (std::multiset<double>::iterator it=l.begin(); it!=l.end(); ++it) {
    tot += (double)*it;
    n++;
  }
  return Times(n,tot);
  //return this->totalViewTime;
}

Times Stats::getTotalPrepTime(unsigned int quant) {
  std::multiset<double> l = this->prepTimes;
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
  for (std::multiset<double>::iterator it=l.begin(); it!=l.end(); ++it) {
    tot += (double)*it;
    n++;
  }
  return Times(n,tot);
}

Times Stats::getTotalNvTime(unsigned int quant) {
  std::multiset<double> l = this->nvTimes;
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
  for (std::multiset<double>::iterator it=l.begin(); it!=l.end(); ++it) {
    tot += (double)*it;
    n++;
  }
  return Times(n,tot);
}

void Stats::incExecViews() { this->execViews++; }

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


std::string Stats::toString() {
    std::ostringstream os;
    os << "[id="              << this->id
       << ";execViews="       << this->execViews
       << ";TEEverify="       << this->TEEverify/this->execViews
       << ";TEEsign="         << this->TEEsign/this->execViews
       << ";TEEprepare="      << this->TEEprepare/this->execViews
       << ";TEEstore="        << this->TEEstore/this->execViews
       << ";TEEaccum="        << this->TEEaccum/this->execViews
       << ";TEEtime="         << this->TEEtime/this->execViews
       << ";totalPrepTime="   << this->totalPrepTime/this->execViews
       << ";totalNvTime="     << this->totalNvTime/this->execViews
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
