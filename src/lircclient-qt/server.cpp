#include "server.h"

#include <iostream>

Server::Server(CEC::ICECAdapter *parser, QObject *parent) : QObject(parent) {
  cec = parser;
  sock = new QTcpServer();
  sock->listen(QHostAddress::Any, 24245);
  
  connect(sock, SIGNAL(newConnection()), this, SLOT(newConnection()));
  
  connections = QList<Conn*>();
}

Server::~Server() {
  sock->close();
}

void Server::newConnection() {
  std::cout << "Got Connection" << std::endl;
  QTcpSocket* tcp = sock->nextPendingConnection();
  
  Conn *conn = new Conn(this, tcp, cec);
  connections.append(conn);
}

// Received a new volume update from the amp via libcec
void Server::volumeUpdate(int vol) {  
  // for all connections
  foreach(Conn *conn, connections) {
    conn->send(vol);
  }
}

void Server::disconnected(Conn* conn) {
  std::cout << "Disconnected" << std::endl;
  connections.removeOne(conn);
  conn->deleteLater();
}
