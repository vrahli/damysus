#ifndef STATS_H
#define STATS_H


#include <chrono>
#include <string>
#include <map>

#include "types.h"

using Time = std::chrono::time_point<std::chrono::steady_clock>;


class Stats {
private:
  unsigned int id        = 0;
  unsigned int execViews = 0;

  double TEEverify       = 0.0;
  double TEEsign         = 0.0;
  double TEEprepare      = 0.0;
  double TEEstore        = 0.0;
  double TEEaccum        = 0.0;
  double TEEtime         = 0.0;

  double totalHandleTime = 0.0; /* total time spent on handling messages */
  double totalViewTime   = 0.0; /* total time spent on views */
  std::map<View,std::tuple<bool,Time,Time>> execTime; // true iff ended

public:
  Stats();

  void setId(unsigned int i);

  void addTEEverify(double v);
  void addTEEsign(double v);
  void addTEEprepare(double v);
  void addTEEstore(double v);
  void addTEEaccum(double v);
  void addTEEtime(double v);

  void addTotalHandleTime(double v);
  void addTotalViewTime(double v);

  void startExecTime(View v, Time t);
  void endExecTime(View v, Time t);
  double getExecTimeAvg();

  double getTotalHandleTime();
  double getTotalViewTime();

  void incExecViews();

  std::string toString();

  friend std::ostream& operator<<(std::ostream& os, const Stats &s);
};


#endif
