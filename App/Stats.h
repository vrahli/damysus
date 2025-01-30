#ifndef STATS_H
#define STATS_H


#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <set>

#include "config.h"
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
  std::vector<double> handleTimes;
  std::vector<double> viewTimes;
  std::vector<double> lprepTimes;
  std::vector<double> lprepbTimes;
  std::vector<double> bprepaTimes;
  std::vector<double> bprepbTimes;
  std::vector<double> bprepcTimes;
  std::vector<double> lvoteTimes;
  std::vector<double> bvoteTimes;
  std::vector<double> laddTimes;
  std::vector<double> baddTimes;
  std::vector<double> decTimes;
  std::vector<double> replyTimes;
  std::vector<double> startTimes;
  std::vector<double> sendTimes;
  std::vector<double> earlyTimes;
  std::vector<double> gen0Times;
  std::vector<double> gen1Times;
  std::vector<double> gen2Times;
  std::vector<double> newTimes;
  std::vector<double> pcommTimes;
  std::vector<double> nvTimes;

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
  double totalLPrepTime  = 0.0; /* time leaders spend preparing */
  double totalLPrepbTime = 0.0; /* time leaders spend preparing */
  double totalBPrepaTime = 0.0; /* time backups sprend on prepares */
  double totalBPrepbTime = 0.0; /* time backups sprend on prepares */
  double totalBPrepcTime = 0.0; /* time backups sprend on prepares */
  double totalLVoteTime  = 0.0; /* time leaders spend voting */
  double totalBVoteTime  = 0.0; /* time backups sprend on voting */
  double totalLAddTime   = 0.0; /* time leaders spend on additional phase */
  double totalBAddTime   = 0.0; /* time backups sprend on additional phase */
  double totalDecTime    = 0.0; /* time nodes spend deciding */
  double totalReplyTime  = 0.0; /* time nodes spend replying */
  double totalStartTime  = 0.0; /* time nodes spend starting */
  double totalSendTime   = 0.0; /* time nodes spend sending */
  double totalEarlyTime  = 0.0; /* time nodes spend handling earlier messages */
  double totalGen0Time   = 0.0; /* time nodes spend generating */
  double totalGen1Time   = 0.0; /* time nodes spend generating */
  double totalGen2Time   = 0.0; /* time nodes spend generating */
  double totalNewTime    = 0.0; /* time nodes spend creating new blocks */
  double totalPCommTime  = 0.0; /* time nodes spend running preCommit */
  double totalNvTime     = 0.0; /* time leaders handling new-views */
  std::map<View,std::tuple<bool,Time,Time>> execTime; // true iff ended

  unsigned int timeouts = 0;
  unsigned int numonepbs = 0;
  unsigned int numonepcs = 0;

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
  void addTotalLPrepTime(double v);
  void addTotalLPrepbTime(double v);
  void addTotalBPrepaTime(double v);
  void addTotalBPrepbTime(double v);
  void addTotalBPrepcTime(double v);
  void addTotalLVoteTime(double v);
  void addTotalBVoteTime(double v);
  void addTotalNvTime(double v);
  void addTotalLAddTime(double v);
  void addTotalBAddTime(double v);
  void addTotalDecTime(double v);
  void addTotalReplyTime(double v);
  void addTotalStartTime(double v);
  void addTotalSendTime(double v);
  void addTotalEarlyTime(double v);
  void addTotalGen0Time(double v);
  void addTotalGen1Time(double v);
  void addTotalGen2Time(double v);
  void addTotalNewTime(double v);
  void addTotalPCommTime(double v);

  void startExecTime(View v, Time t);
  void endExecTime(View v, Time t);
  double getExecTimeAvg();

  unsigned int getCryptoSignNum();
  unsigned int getCryptoVerifNum();
  double getCryptoSignTime();
  double getCryptoVerifTime();

  Times getTotalHandleTime(unsigned int quant);
  Times getTotalViewTime(unsigned int quant);
  Times getTotalLPrepTime(unsigned int quant);
  Times getTotalLPrepbTime(unsigned int quant);
  Times getTotalBPrepaTime(unsigned int quant);
  Times getTotalBPrepbTime(unsigned int quant);
  Times getTotalBPrepcTime(unsigned int quant);
  Times getTotalLVoteTime(unsigned int quant);
  Times getTotalBVoteTime(unsigned int quant);
  Times getTotalNvTime(unsigned int quant);
  Times getTotalLAddTime(unsigned int quant);
  Times getTotalBAddTime(unsigned int quant);
  Times getTotalDecTime(unsigned int quant);
  Times getTotalReplyTime(unsigned int quant);
  Times getTotalStartTime(unsigned int quant);
  Times getTotalSendTime(unsigned int quant);
  Times getTotalEarlyTime(unsigned int quant);
  Times getTotalGen0Time(unsigned int quant);
  Times getTotalGen1Time(unsigned int quant);
  Times getTotalGen2Time(unsigned int quant);
  Times getTotalNewTime(unsigned int quant);
  Times getTotalPCommTime(unsigned int quant);

  unsigned int getTimeouts();
  unsigned int getNumOnePBs();
  unsigned int getNumOnePCs();

  void incExecViews();
  void incTimeouts();
  void incNumOnePBs();
  void incNumOnePCs();

  void printAllTimes();
  std::string toString();

  friend std::ostream& operator<<(std::ostream& os, const Stats &s);
};


#endif
