#include "lirc.h"

#include <fcntl.h>
#include <stdlib.h>
#include <iostream>
#include <QCoreApplication>

// Got a volume change event 
// (could be called from libcec thread)
void Lirc::displayVolume(int vol) {  
  if (vol & 0x80) {
    // mute
    vol = 0;
  }
  
  if (vol > 100) vol = 0;
  //std::cout << "Changed volume: " << vol << std::endl;
  emit volume(vol);
}

void Lirc::update(void) {
  char *code;
  char *c;
  int ret = -1;

  //std::cout << "Lirc activity" << std::endl;

  while(lirc_nextcode(&code)==0 && code!=NULL)
  {
      while((ret=lirc_code2char(config,code,&c))==0 && c!=NULL)
      {
	  // Device select
	  if (strcmp(c,"CEC_SELECT")==0) {

	      if (parser->GetActiveSource() == CEC::CECDEVICE_RECORDINGDEVICE1) {
	      //if (parser->IsLibCECActiveSource()) {
		  // switch to playstation
		  //parser->SetStreamPath(CEC::CECDEVICE_PLAYBACKDEVICE1);
		  // pretend to be the TV
		  uint16_t ps3 = parser->GetDevicePhysicalAddress(CEC::CECDEVICE_PLAYBACKDEVICE1);
		  CEC::cec_command cmd;
		  CEC::cec_command::Format(cmd, CEC::CECDEVICE_TV, CEC::CECDEVICE_BROADCAST, CEC::CEC_OPCODE_SET_STREAM_PATH);
		  cmd.parameters.PushBack((uint8_t) ((ps3 >> 8) & 0xFF));
		  cmd.parameters.PushBack((uint8_t) (ps3 & 0xFF));
		  parser->Transmit(cmd);

		  lirc_setmode(config, "ps3");
	      } else {
		  // switch to mythtv
		      //parser->SetStreamPath(CEC::CECDEVICE_RECORDINGDEVICE1);
		      uint16_t pc = parser->GetDevicePhysicalAddress(CEC::CECDEVICE_RECORDINGDEVICE1);
		      CEC::cec_command cmd;
		      CEC::cec_command::Format(cmd, CEC::CECDEVICE_TV, CEC::CECDEVICE_BROADCAST, CEC::CEC_OPCODE_SET_STREAM_PATH);
		      cmd.parameters.PushBack((uint8_t) ((pc >> 8) & 0xFF));
		      cmd.parameters.PushBack((uint8_t) (pc & 0xFF));
		      parser->Transmit(cmd);
		      parser->SetActiveSource(CEC::CEC_DEVICE_TYPE_RECORDING_DEVICE);

		  lirc_setmode(config, "myth");
	      }
	  } 
	  // Power controls
	  else if (strcmp(c,"CEC_POWER")==0) {
	      CEC::cec_power_status iPower = parser->GetDevicePowerStatus(CEC::CECDEVICE_TV);
	      if (iPower == CEC::CEC_POWER_STATUS_STANDBY) {
		  parser->PowerOnDevices(CEC::CECDEVICE_TV);
		  parser->SetActiveSource(CEC::CEC_DEVICE_TYPE_RECORDING_DEVICE);
		  lirc_setmode(config, "myth");
	      } else {
		  parser->StandbyDevices(CEC::CECDEVICE_BROADCAST);
	      }
	  } 
	  // Amp controls
	  else if (strcmp(c,"CEC_VOLUP")==0) {
	      displayVolume(parser->VolumeUp());
	  } else if (strcmp(c,"CEC_VOLDOWN")==0) {
	      displayVolume(parser->VolumeDown());
	  } else if (strcmp(c,"CEC_MUTE")==0) {
	     displayVolume(parser->MuteAudio());
	  }
	  // Playstation controls
	  else if (strcmp(c,"CEC_UP")==0) {
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_UP, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_DOWN")==0) {
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_DOWN, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_LEFT")==0) {
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_LEFT, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_RIGHT")==0) {
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_RIGHT, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_OK")==0) {
	      // x
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_SELECT, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_BACK")==0) {
	      // circle
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_EXIT, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_MENU")==0) {
	      // ps button
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_ROOT_MENU, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_SETUP")==0) {
	      // triangle
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_SETUP_MENU, false);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  } else if (strcmp(c,"CEC_HOLD")==0) {
	      // hold ps button
	      parser->SendKeypress(CEC::CECDEVICE_PLAYBACKDEVICE1, CEC::CEC_USER_CONTROL_CODE_ROOT_MENU, false);
	      sleep(1);
	      parser->SendKeyRelease(CEC::CECDEVICE_PLAYBACKDEVICE1, false);
	  }
      }
      free(code);
      if(ret==-1) break;
  }
  if(ret==-1)
  {
	  // something went badly wrong
	  std::cout << "something went wrong" << std::endl;
	  QCoreApplication::exit(EXIT_FAILURE);
	  return;
  }
}

Lirc::~Lirc() {
  lirc_freeconfig(config);
  
  lirc_deinit();
}

Lirc::Lirc(CEC::ICECAdapter *parser, QObject *parent) : QObject(parent) {
  
  this->parser = parser;
  
  sock = lirc_init("mythtv",1);
  if (sock == -1) {
      std::cout << "Lirc initialisation failed" << std::endl;
      QCoreApplication::exit(EXIT_FAILURE);
  }
  
  std::cout << "Lirc initialised" << std::endl;

  if(lirc_readconfig(NULL,&config,NULL)==0)
  {
    fcntl(sock,F_SETOWN,getpid());
    int flags = fcntl(sock,F_GETFL,0);
    if(flags != -1)
    {
	    fcntl(sock,F_SETFL,flags|O_NONBLOCK);
    } else {
      std::cout << "Lirc cannot be set to be non-blocking" << std::endl;
    }
    sn = new QSocketNotifier(sock, QSocketNotifier::Read, 0);
    connect(sn, SIGNAL(activated(int)), this, SLOT(update()));
  } else {
    std::cout << "Lirc cannot read config" << std::endl;
  }
  
  std::cout << "Lirc connected" << std::endl;
  
  // The mode switching allows the remote to usually control mythtv, but when in ps3 mode, some keys are forwarded over cec
  // Make sure that #!lircrcd is at the start of the .lircrc file to enable lirc_setmode to work
  lirc_setmode(config, "myth");
}
