#ifndef CONN_H
#define CONN_H

#include <QTcpSocket>

class Server;

class Conn : public QObject
{
	Q_OBJECT
public:
	Conn(Server *server, QTcpSocket *conn);
	~Conn();
	void send(int);
public slots:
	void req();
	void disconnected();
	
private:
	Server *server;
	QTcpSocket *conn;
};

#endif // SERVER_H
