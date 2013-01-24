#ifndef LIRC_H
#define LIRC_H

#include <QObject>
#include <QSocketNotifier>

#include <lirc/lirc_client.h>

#include "../../include/cec.h"

class Lirc : public QObject
{
	Q_OBJECT
public:
	Lirc(CEC::ICECAdapter *parser, QObject * parent = 0);
	~Lirc();
signals:
	void volume(int);
public slots:
	void update(void);
	void displayVolume(int vol);
private:  
	int sock;
	QSocketNotifier *sn;
	struct lirc_config *config;
	CEC::ICECAdapter *parser;
};

#endif // LIRC_H
