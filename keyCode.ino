#include "global.h"

#include "src/lib/ArduinoProcessScheduler/src/ProcessScheduler.h"
#include "src/Common_code/Network/network.h"
#include "src/Common_code/Network/TcpServer.h"
#include "src/Common_code/Network/HttpServer.h"
#include "src/Common_code/Network/TelnetServer.h"
#include "src/Common_code/Network/servlets/servlets.h"
#include "src/Common_code/WatchdogProcess.h"

#include "src/Common_code/TLE8457_serial/TLE8457_serial_lib.h"
#include "src/Common_code/TLE8457_serial/tIncomingFrameHanlder.h"

Scheduler sched;
tWatchdogProcess WatchdogProcess(sched);

#if CONFIG_TLE8457_COMM_LIB
tIncomingFrameHanlder IncomingFrameHandler;
CommRecieverProcess CommReciever(sched,EEPROM.read(EEPROM_DEVICE_ID_OFFSET));
CommSenderProcess CommSender(sched,EEPROM.read(EEPROM_DEVICE_ID_OFFSET),EEPROM.read(EEPROM_DEVICE_ID_OFFSET));

void COMM_SERIAL_EVENT() {
  CommReciever.serialEvent();
}
#endif //CONFIG_TLE8457_COMM_LIB

#if CONFIG_NETWORK

tNetwork Network;
tTcpServerProcess TcpServerProcess(sched, 0);	// watchdog disabled

#if CONFIG_HTTP_SERVER
tHttpServer HttpServer;
#endif

#if CONFIG_TELNET_SERVER
extern tTelnetServer TelnetServer;
#endif

#if CONFIG_HTTP_SERVER
tHttpServlet * ServletFactory(String *pRequestBuffer)
{
#if CONFIG_HTTP_SERVLET_OUTCONTROL_JS
   if (pRequestBuffer->startsWith("/outputState")) return new tOutputStateServlet();
#endif //CONFIG_HTTP_SERVLET_OUTCONTROL_JS
   return NULL;
}
#endif

#endif // CONFIG_NETWORK


void setup() {
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.begin(115200);
  while (!DEBUG_SERIAL);
  DEBUG_SERIAL.print(F("START, v"));
  DEBUG_SERIAL.println(FW_VERSION);
#endif

  if (EEPROM.read(EEPROM_CANNARY_OFFSET) != EEPROM_CANNARY)
    SetDefaultEEPromValues();

  COMM_SERIAL.begin(9600);
  while (!COMM_SERIAL);

#if CONFIG_TLE8457_COMM_LIB
  CommSender.add();
  CommReciever.add();
#endif //CONFIG_TLE8457_COMM_LIB

#if CONFIG_NETWORK
  Network.init();
  TcpServerProcess.add(true);
  
#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.println(F("START Tcp "));
#endif

#endif // CONFIG_NETWORK

  WatchdogProcess.add(true);

#ifdef DEBUG_SERIAL
  DEBUG_SERIAL.print(F("Free RAM: "));
  DEBUG_SERIAL.println(getFreeRam());
  DEBUG_SERIAL.println(F("SYSTEM INITIALIZED"));
#endif
}

void loop() {
  sched.run();
}
