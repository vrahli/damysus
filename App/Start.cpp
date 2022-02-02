#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>

#include "Message.h"
#include "Nodes.h"


int main(int argc, char const *argv[]) {
  struct sockaddr_in address;
  int sock;
  struct sockaddr_in serv_addr;
  KeysFun kf;

  // Geting inputs
  char conf[CONF_FILE_SIZE];
  if (argc > 1) { sscanf(argv[1], "%s", conf); }
  std::cout << "[configuration file is: " << conf << "]" << std::endl;

  // pid is the node we want to send messages to
  int pid = 0;
  Nodes nodes = Nodes(kf,conf);

  NodeInfo * nfo = nodes.find(pid);

  if (nfo == NULL) {
    std::cout << "couldn't find info about " << pid << " in conf file" << std::endl;
  } else {

    std::cout << "found info about " << pid <<  " in conf file" << std::endl;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      std::cout << "Socket creation error" << std::endl;
      return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(nfo->getPort());

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, nfo->getHost().c_str(), &serv_addr.sin_addr)<=0) {
      std::cout << "Invalid address/Address not supported" << std::endl;
      return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
      std::cout << "Connection Failed" << std::endl;
      return -1;
    }

    std::cout << "connected" << std::endl;

    //usleep(2*1000*1000);

    Transaction transaction = Transaction();
    Signs signs = Signs();
    Message<Transaction> msg(HDR_START,transaction,signs);

    char cmsg[sizeof(Message<Transaction>)];
    msg.serialize(cmsg);
    int valsent = send(sock, cmsg, sizeof(Message<Transaction>), 0);
    std::cout << "message sent (" << valsent << ")" << std::endl;
    //std::cout << "msg: --" << msg << "--" << std::endl;

    //usleep(2*1000*1000);
  }

  return 0;
}
