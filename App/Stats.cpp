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

  this->totalHandleTime = 0.0;
  this->totalViewTime   = 0.0;
}

void Stats::setId(unsigned int i) { this->id = i; }

void Stats::addTEEverify(double v)  { this->TEEverify  += v; }
void Stats::addTEEsign(double v)    { this->TEEsign    += v; }
void Stats::addTEEprepare(double v) { this->TEEprepare += v; }
void Stats::addTEEstore(double v)   { this->TEEstore   += v; }
void Stats::addTEEaccum(double v)   { this->TEEaccum   += v; }
void Stats::addTEEtime(double v)    { this->TEEtime    += v; }

void Stats::addTotalHandleTime(double v) { this->totalHandleTime += v; }
void Stats::addTotalViewTime(double v)   { this->totalViewTime   += v; }

double Stats::getTotalHandleTime() { return this->totalHandleTime; }
double Stats::getTotalViewTime()   { return this->totalViewTime;   }

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
       << ";totalHandleTime=" << this->totalHandleTime/this->execViews
       << ";totalViewTime="   << this->totalViewTime/this->execViews
       << "]";
    return os.str();
}

std::ostream& operator<<(std::ostream& os, const Stats& s) {
    os << std::to_string(s.id);
    return os;
}
