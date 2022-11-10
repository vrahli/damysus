#ifndef STATS_H
#define STATS_H


#include <chrono>
#include <string>
#include <map>
#include <set>

#include "types.h"

using Time = std::chrono::time_point<std::chrono::steady_clock>;

struct Times {
  unsigned int n; // number of values used to compute the sum 'tot'
  double tot;
  Times() { n = 0; tot = 0.0; }
  Times(unsigned int n, double tot): n(n),tot(tot) {}
};


class Stats {
private:
  std::multiset<double> handleTimes;
  std::multiset<double> viewTimes;
  std::multiset<double> prepTimes;
  std::multiset<double> nvTimes;

  unsigned int id        = 0;
  unsigned int execViews = 0;

  double TEEverify       = 0.0;
  double TEEsign         = 0.0;
  double TEEprepare      = 0.0;
  double TEEstore        = 0.0;
  double TEEaccum        = 0.0;
  double TEEtime         = 0.0;

  unsigned int cryptoSignNum  = 0;
  unsigned int cryptoVerifNum = 0;
  double cryptoSignTime  = 0.0;
  double cryptoVerifTime = 0.0;

  double totalHandleTime = 0.0; /* total time spent on handling messages */
  double totalViewTime   = 0.0; /* total time spent on views */
  double totalPrepTime   = 0.0; /* time leaders spend preparing */
  double totalNvTime     = 0.0; /* time leaders handling new-views */
  std::map<View,std::tuple<bool,Time,Time>> execTime; // true iff ended

public:
  Stats();

  void setId(unsigned int i);
  PID getId();

  void addTEEverify(double v);
  void addTEEsign(double v);
  void addTEEprepare(double v);
  void addTEEstore(double v);
  void addTEEaccum(double v);
  void addTEEtime(double v);

  void addCryptoSignTime(double v);
  void addCryptoVerifTime(double v);

  void addTotalHandleTime(double v);
  void addTotalViewTime(double v);
  void addTotalPrepTime(double v);
  void addTotalNvTime(double v);

  void startExecTime(View v, Time t);
  void endExecTime(View v, Time t);
  double getExecTimeAvg();

  unsigned int getCryptoSignNum();
  unsigned int getCryptoVerifNum();
  double getCryptoSignTime();
  double getCryptoVerifTime();

  Times getTotalHandleTime(unsigned int quant);
  Times getTotalViewTime(unsigned int quant);
  Times getTotalPrepTime(unsigned int quant);
  Times getTotalNvTime(unsigned int quant);

  void incExecViews();

  std::string toString();

  friend std::ostream& operator<<(std::ostream& os, const Stats &s);
};


#endif
