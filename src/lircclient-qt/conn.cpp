#include "conn.h"
#include "server.h"

#include <QByteArray>

Conn::Conn(Server *server, QTcpSocket *conn, CEC::ICECAdapter *cec) : QObject(server) {
  this->server = server;
  this->conn = conn;
  this->cec = cec;
  
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
  
  // check that the TV is on
  CEC::cec_power_status iPower = cec->GetDevicePowerStatus(CEC::CECDEVICE_TV);
  if (iPower == CEC::CEC_POWER_STATUS_STANDBY) {
    conn->write("Off\n");
  } else {
    foreach(const char c, ba) {
      if (c == 'U') {
	cec->VolumeUp();
      } else if (c == 'D') {
	cec->VolumeDown();
      } else if (c == 'M') {
	cec->MuteAudio();
	conn->write("Mute\n");
      }
    }
  }
}

void Conn::send(int vol) {
  conn->write(QString::number(vol).toLatin1() + "\n");
}
