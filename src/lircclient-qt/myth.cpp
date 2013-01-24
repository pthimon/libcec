#include "myth.h"

#include <iostream>
#include <QHostAddress>

Myth::Myth(QObject* parent) : QObject(parent) {
  connectToMyth();
}

Myth::~Myth() {
  mythSock->close();
}

void Myth::connected() {
  mythIsConnected = true;
  std::cout << "Connected to mythfrontend" << std::endl;
}

void Myth::disconnected() {
  mythIsConnected = false;
  std::cout << "Disconnected from mythfrontend" << std::endl;
}

void Myth::displayVolume(int vol) {
  QString str = QString("play volume %1%\n").arg(vol);
  write(str);
}

void Myth::write(QString str) {
  //std::cout << "Writing to mythfrontend: " << str.toStdString() << std::endl;
  if (mythSock->isValid() && mythIsConnected) {
    mythSock->write(str.toLatin1());
  } else {
    std::cout << "Mythfrontend has disappeared, reconnecting..." << std::endl;
    mythSock->abort();
    connectToMyth();
  }
}

void Myth::read() {
  // read and discard response from mythfrontend
  mythSock->readAll();
}

void Myth::error(QAbstractSocket::SocketError error) {
  std::cout << "Got error! " << error << std::endl;
}

void Myth::connectToMyth() {
  std::cout << "Connecting to mythfrontend" << std::endl;
  mythSock = new QTcpSocket(this);
  mythSock->connectToHost(QHostAddress::LocalHost, 6546);
  
  connect(mythSock, SIGNAL(connected()), this, SLOT(connected()));
  connect(mythSock, SIGNAL(disconnected()), this, SLOT(disconnected()));
  connect(mythSock, SIGNAL(readyRead()), this, SLOT(read()));
  connect(mythSock, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error(QAbstractSocket::SocketError)));
}  
