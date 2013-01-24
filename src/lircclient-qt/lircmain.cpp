#include <iostream>
#include <QApplication>

#include "lirc.h"
#include "myth.h"
#include "server.h"

Lirc *lirc;
Myth *myth;
Server *server;

int CecVolCommand(void *UNUSED(cbParam), const cec_command command)
{
  if (command.opcode == CEC_OPCODE_REPORT_AUDIO_STATUS) {
    // when the audio system reports the audio status to the TV, display the volume through mythfrontend	
    int vol = command.parameters[0];
    //std::cout << "Got volume: " << vol << std::endl;
    
    lirc->displayVolume(vol);
  }
  return 0;
}

int lircMain(int argc, char *argv[], ICECAdapter *parser) {
  QCoreApplication a(argc,argv);
  
  lirc = new Lirc(parser, &a);
  myth = new Myth(&a);
  server = new Server(parser, &a);
  QObject::connect(lirc, SIGNAL(volume(int)), myth, SLOT(displayVolume(int)));
  QObject::connect(lirc, SIGNAL(volume(int)), server, SLOT(volumeUpdate(int)));
  
  return a.exec();
}