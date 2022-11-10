#include <iostream>
#include <cstring>
#include <fstream>

#include "config.h"
#include "Nodes.h"


void Nodes::addNode(PID id, KEY pub, HOST host, PORT rport, PORT cport) {
  if (DEBUG) { std::cout << KMAG << "adding " << id << " to list of nodes" << KNRM << std::endl; }
  NodeInfo hp(id,pub,host,rport,cport);
  this->nodes[id]=hp;
}


void Nodes::addNode(PID id, HOST host, PORT rport, PORT cport) {
  if (DEBUG) { std::cout << KMAG << "adding " << id << " to list of nodes" << KNRM << std::endl; }
  NodeInfo hp(id,host,rport,cport);
  this->nodes[id]=hp;
}


NodeInfo * Nodes::find(PID id) {
  std::map<PID,NodeInfo>::iterator it = nodes.find(id);

  if (it != nodes.end()) {
    if (DEBUG) { std::cout << KMAG << "found a corresponding NodeInfo entry" << KNRM << std::endl; }
    return &(it->second);
  } else { return NULL; }
}


void Nodes::setPub(PID id, KEY pub) {
  std::map<PID,NodeInfo>::iterator it = nodes.find(id);

  if (it != nodes.end()) {
    if (DEBUG) { std::cout << KMAG << "found a corresponding NodeInfo entry" << KNRM << std::endl; }
    NodeInfo nfo = it->second;
    nfo.setPub(pub);
    this->nodes[id]=nfo;
  }
}


std::set<PID> Nodes::getIds() {
  std::map<PID,NodeInfo>::iterator it = nodes.begin();
  std::set<PID> l = {};
  while (it != nodes.end()) { l.insert(it->first); it++; }
  return l;
}


void Nodes::printNodes() {
  std::map<PID,NodeInfo>::iterator it = nodes.begin();

  while (it != nodes.end()) {
    PID id = it->first;
    NodeInfo nfo = it->second;

    if (DEBUG) std::cout << KMAG << "id: " << id << KNRM << std::endl;
    it++;
  }
}

int Nodes::numNodes() {
  std::map<PID,NodeInfo>::iterator it = nodes.begin();

  int count = 0;
  while (it != nodes.end()) { count++; it++; }
  return count;
}


void printIds(std::list<PID> l) {
  std::list<PID>::iterator it = l.begin();

  while (it != l.end()) {
      if (DEBUG) std::cout << KMAG << "id: " << *it << KNRM << std::endl;
    it++;
  }
}


Nodes::Nodes() {}


Nodes::Nodes(std::string filename, unsigned int numNodes) {
  std::ifstream inFile(filename);
  char oneline[MAXLINE];
  char delim[] = " ";
  char *token;
  unsigned int added = 0;

  if (DEBUG) std::cout << KMAG << "parsing configuration file" << KNRM << std::endl;

  while (inFile && added < numNodes) {
    inFile.getline(oneline,MAXLINE);
    token = strtok(oneline,delim);

    if (token) {
      // id
      int id = atoi(token+3);

      // host
      token=strtok(NULL,delim);
      HOST host = token+5;

      // replica port
      token=strtok(NULL,delim);
      PORT rport = atoi(token+5);

      // client port
      token=strtok(NULL,delim);
      PORT cport = atoi(token+5);

      if (DEBUG) std::cout << KMAG << "id: " << id << "; host: " << host << "; port: " << rport << "; port: " << cport << KNRM << std::endl;

      addNode(id,host,rport,cport);
      added++;
    }
  }

  if (DEBUG) std::cout << KMAG << "closing configuration file" << KNRM << std::endl;
  inFile.close();
  if (DEBUG) std::cout << KMAG << "done parsing the configuration file" << KNRM << std::endl;
}
