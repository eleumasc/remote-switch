#ifndef WEBAPP_H_
#define WEBAPP_H_

#define WEBDUINO_NO_IMPLEMENTATION

#include <Arduino.h>
#include <WebServer.h>

class WebApp {

  static int lastStatus;

  static bool requestingReset;

  static bool resetRequired;

  static void httpSeeOtherSuccess(WebServer &server);

  static void httpSeeOtherFail(WebServer &server);

  static void ip2string(uint32_t ip, char string[16]);

  static uint32_t string2ip(const char* string);

  WebApp() {}

public:

  static bool isResetRequired();

  static void addAllCommands(const WebServer& ws);

  static void wsDefaultCommand(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsStatus(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsSwitchStatus(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsPassword(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsDeviceName(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsIpConfig(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsRedirect(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsReset(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

  static void wsHardReset(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete);

};

#endif
