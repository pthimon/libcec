#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include <lirc/lirc_client.h>

int		     g_mythfrontendSock;
PLATFORM::CMutex     g_mutex; 

// Display the volume on mythfrontend
void displayVolume(int vol) {  
  if (vol & 0x80) {
    // mute
    vol = 0;
  }
  
  char buffer[256];
  sprintf(buffer, "play volume %d%%\n", vol);
  
  CLockObject lock(g_mutex);
  write(g_mythfrontendSock,buffer,strlen(buffer));  
  read(g_mythfrontendSock,buffer,255);
}

int CecVolCommand(void *UNUSED(cbParam), const cec_command command)
{
  if (command.opcode == CEC_OPCODE_REPORT_AUDIO_STATUS) {
    // when the audio system reports the audio status to the TV, display the volume through mythfrontend	
    int vol = command.parameters[0];
    CStdString cmd;
    cmd.Format("Got volume: %d", vol);
    PrintToStdOut(cmd.c_str());
    displayVolume(vol);
  }
  return 0;
}

// Open a connection to the mythfrontend control interface
void connectMythFrontend() {
  struct sockaddr_in serv_addr;
  struct hostent *server;
  g_mythfrontendSock = socket(AF_INET, SOCK_STREAM, 0);
  server = gethostbyname("localhost");
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
  serv_addr.sin_port = htons(6546);
  
  if (connect(g_mythfrontendSock,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
    PrintToStdOut("ERROR connecting");
  
  char buffer[256];  
  bzero(buffer,256);
  int n = read(g_mythfrontendSock,buffer,255);
  if (n < 0) 
    PrintToStdOut("ERROR reading from socket");
  else
    PrintToStdOut("Connected to Mythfrontend");
}

void lircMain(ICECAdapter *parser) {
  
  int fd = lirc_init("mythtv",1);
  if (fd == -1) {
      cout << "Lirc initialisation failed" << endl;
  }
  
  connectMythFrontend();

  struct lirc_config *config;
  if(lirc_readconfig(NULL,&config,NULL)==0)
  {
      char *code;
      char *c;
      int ret;

	  // The mode switching allows the remote to usually control mythtv, but when in ps3 mode, some keys are forwarded over cec
	  // Make sure that #!lircrcd is at the start of the .lircrc file to enable lirc_setmode to work
      lirc_setmode(config, "myth");

      while(lirc_nextcode(&code)==0)
      {
          if(code==NULL) continue;
          while((ret=lirc_code2char(config,code,&c))==0 && c!=NULL)
          {
              // Device select
              if (strcmp(c,"CEC_SELECT")==0) {

                  if (parser->GetActiveSource() == CEC::CECDEVICE_RECORDINGDEVICE1) {
                      // switch to playstation
                      parser->SetStreamPath(CEC::CECDEVICE_PLAYBACKDEVICE1);

                      lirc_setmode(config, "ps3");
                  } else {
                      // switch to mythtv
                	  parser->SetStreamPath(CEC::CECDEVICE_RECORDINGDEVICE1);
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
      lirc_freeconfig(config);
  }
  
  close(g_mythfrontendSock);
  
  lirc_deinit();
}