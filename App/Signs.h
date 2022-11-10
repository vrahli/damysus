#ifndef SIGNS_H
#define SIGNS_H

#include <set>

#include "Sign.h"
#include "Nodes.h"
#include "RData.h"
#include "Stats.h"

#include "salticidae/stream.h"


class Signs {
  private:
    unsigned int size = 0;
    Sign signs[MAX_NUM_SIGNATURES];

  public:
    Signs();
    Signs(Sign sign);
    Signs(unsigned int size, Sign signs[MAX_NUM_SIGNATURES]);
    Signs(salticidae::DataStream &data);

    unsigned int getSize();

    Sign get(unsigned int n);
    void add(Sign sign);
    void addUpto(Signs others, unsigned int n);

    bool verify(Stats &stats, PID id, Nodes nodes, std::string s);

    std::set<PID> getSigners();
    std::string printSigners();

    void serialize(salticidae::DataStream &data) const;
    void unserialize(salticidae::DataStream &data);

    std::string prettyPrint();
    std::string toString();

    bool hasSigned(PID p);

    bool operator<(const Signs& s) const;
    bool operator==(const Signs& s) const;
};


#endif
