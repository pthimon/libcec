#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

#include "../../include/cec.h"

#include "conn.h"

class Server : public QObject
{
	Q_OBJECT
public:
	Server(CEC::ICECAdapter *parser, QObject * parent = 0);
	~Server();
	
	void disconnected(Conn*);
public slots:
	void newConnection();
	void volumeUpdate(int);
	
private:
	QTcpServer* sock;
	QList<Conn*> connections;
	CEC::ICECAdapter *cec;
};

#endif // SERVER_H
