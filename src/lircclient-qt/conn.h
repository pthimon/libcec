#ifndef CONN_H
#define CONN_H

#include <QTcpSocket>

#include "../../include/cec.h"

class Server;

class Conn : public QObject
{
	Q_OBJECT
public:
	Conn(Server *server, QTcpSocket *conn, CEC::ICECAdapter *cec);
	~Conn();
	void send(int);
public slots:
	void req();
	void disconnected();
	
private:
	Server *server;
	QTcpSocket *conn;
	CEC::ICECAdapter *cec;
};

#endif // SERVER_H
