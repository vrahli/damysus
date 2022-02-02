#ifndef HOSTPORT_H
#define HOSTPORT_H


#include <string>

#include "types.h"
#include "KeysFun.h"


typedef std::string HOST;


class NodeInfo {
 private:
  PID pid;
  KEY pub;
  HOST host;
  PORT rport; // replica port to listen to replicas
  PORT cport; // client port to listen to clients

 public:
  NodeInfo();
  NodeInfo(PID pid, HOST host, PORT rport, PORT cport);
  NodeInfo(PID pid, KEY pub, HOST host, PORT rport, PORT cport);

  PID getId();
  KEY getPub();
  HOST getHost();
  PORT getRPort();
  PORT getCPort();

  void setPub(KEY pub);

  std::string toString();

  bool operator<(const NodeInfo& hp) const;
};


#endif
