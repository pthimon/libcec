#ifndef MYTH_H
#define MYTH_H

#include <QObject>
#include <QTcpSocket>

class Myth : public QObject
{
	Q_OBJECT
public:
	Myth(QObject * parent = 0);
	~Myth();
public slots:
	void displayVolume(int vol);
	void connected();
	void disconnected();
	void read();
	void error(QAbstractSocket::SocketError error);
private:
	void write(QString);
	void connectToMyth();

	QTcpSocket *mythSock;
	bool mythIsConnected;
};

#endif // MYTH_H
