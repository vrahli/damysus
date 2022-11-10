#ifndef AUTHS_H
#define AUTHS_H

#include <set>

#include "Auth.h"
#include "Nodes.h"

#include "salticidae/stream.h"


class Auths {
  private:
    unsigned int size = 0;
    Auth auths[MAX_NUM_SIGNATURES];

  public:
    Auths();
    Auths(Auth auth);
    Auths(unsigned int size, Auth auths[MAX_NUM_SIGNATURES]);
    Auths(salticidae::DataStream &data);

    unsigned int getSize();

    Auth get(unsigned int n);
    void add(Auth auth);
    void addUpto(Auths others, unsigned int n);

    bool in(PID pid);
    void addNew(Auth auth);
    void addNew(Auths auths);

    bool verify(std::string secret, std::string s);

    std::set<PID> getSigners();
    std::string printSigners();

    void serialize(salticidae::DataStream &data) const;
    void unserialize(salticidae::DataStream &data);

    std::string prettyPrint();
    std::string toString();

    bool hasSigned(PID p);

    bool operator<(const Auths& s) const;
    bool operator==(const Auths& s) const;
};


#endif
