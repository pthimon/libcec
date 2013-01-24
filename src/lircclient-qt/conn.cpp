#include "conn.h"
#include "server.h"

#include <QByteArray>

Conn::Conn(Server *server, QTcpSocket *conn) : QObject(server) {
  this->server = server;
  this->conn = conn;
  
  connect(conn, SIGNAL(readyRead()), this, SLOT(req()));
  connect(conn, SIGNAL(disconnected()), this, SLOT(disconnected()));
}

Conn::~Conn() {
  
}

void Conn::disconnected() {
  server->disconnected(this);
  conn->deleteLater();
}

void Conn::req() {
  // parse req from conn
  QByteArray ba = conn->readAll();
  
  foreach(const char c, ba) {
    if (c == 'U') {
      server->emitVolumeUp();
    } else if (c == 'D') {
      server->emitVolumeDown();
    }
  }
}

void Conn::send(int vol) {
  conn->write(QString::number(vol).toLatin1() + "\n");
}
