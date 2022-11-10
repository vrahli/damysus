#ifndef NODES_H
#define NODES_H


#include <map>
#include <set>
#include <list>

#include "types.h"
#include "NodeInfo.h"
#include "KeysFun.h"


class Nodes {
 private:
  std::map<PID,NodeInfo> nodes;

 public:
  Nodes();
  Nodes(std::string filename, unsigned int numNodes);

  void addNode(PID id, KEY pub, HOST host, PORT rport, PORT cport);
  void addNode(PID id, HOST host, PORT rport, PORT cport);

  void setPub(PID id, KEY pub);

  NodeInfo * find(PID id);
  std::set<PID> getIds();

  void printNodes();
  int numNodes();
};


#endif
