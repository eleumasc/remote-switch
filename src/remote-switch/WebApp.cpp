#include "WebApp.h"
#include "Switch.h"
#include "Config.h"

void WebApp::httpSeeOtherSuccess(WebServer &server) {
  lastStatus = 1;
  server.httpSeeOther("redirect");
}

void WebApp::httpSeeOtherFail(WebServer &server) {
  lastStatus = 2;
  server.httpSeeOther("redirect");
}

void WebApp::ip2string(uint32_t ip, char string[16]) {
  int s = 0;
  for (int i = 0; i < 4; ++i, ip >>= 8) {
    if (i != 0) {
      string[s++] = '.';
    }
    byte thisByte = (byte) (ip & 0xff);
    if (thisByte > 99) {
      string[s++] = (char) (((thisByte / 100) % 10) + '0');
    }
    if (thisByte > 9) {
      string[s++] = (char) (((thisByte / 10) % 10) + '0');
    }
    string[s++] = (char) ((thisByte % 10) + '0');
  }
  string[s] = '\0';
}

uint32_t WebApp::string2ip(const char* string) {
  uint32_t ip = 0;
  int byteCount = 0;
  int s = 0;
  while (string[s]) {
    uint32_t thisByte = 0;
    bool firstDigit = true;
    while (string[s] && string[s] != '.') {
      if (string[s] < '0' || string[s] > '9') {
        return 0;
      }
      thisByte = thisByte * 10 + (string[s++] - '0');
      firstDigit = false;
      if (thisByte > 0xff) {
        return 0;
      }
    }
    if (firstDigit) {
      return 0;
    }
    ip |= thisByte << (8 * byteCount++);
    if (string[s]) ++s;
  }
  if (byteCount != 4) {
    return 0;
  }
  return ip;
}

bool WebApp::isResetRequired() {
  return resetRequired;
}

void WebApp::addAllCommands(const WebServer& ws) {
  ws.setDefaultCommand(&wsDefaultCommand);
  ws.addCommand("status", &wsStatus);
  ws.addCommand("switchStatus", &wsSwitchStatus);
  ws.addCommand("password", &wsPassword);
  ws.addCommand("deviceName", &wsDeviceName);
  ws.addCommand("ipConfig", &wsIpConfig);
  ws.addCommand("redirect", &wsRedirect);
  ws.addCommand("reset", &wsReset);
  ws.addCommand("hardReset", &wsHardReset);
}

P(WS_DEFAULT_COMMAND) = "<!doctypehtml><title>Remote Switch</title><meta content=\"width=device-width,initial-scale=1\"name=viewport><style>html{box-sizing:border-box}*,*:before,*:after{box-sizing:inherit} html{-ms-text-size-adjust:100%;-webkit-text-size-adjust:100%}body{margin:0} header{display:block} button,input{font:inherit;margin:0} button,input{overflow:visible}button{text-transform:none} button,[type=submit]{-webkit-appearance:button} button::-moz-focus-inner,[type=submit]::-moz-focus-inner{border-style:none;padding:0} button:-moz-focusring,[type=submit]:-moz-focusring{outline:1px dotted ButtonText} [type=checkbox]{padding:0} ::-webkit-file-upload-button{-webkit-appearance:button;font:inherit} html,body{font-family:Verdana,sans-serif;font-size:15px;line-height:1.5}html{overflow-x:hidden} h1{font-size:36px} h1{font-family:\"Segoe UI\",Arial,sans-serif;font-weight:400;margin:10px 0} .w3-btn{border:none;display:inline-block;padding:8px 16px;vertical-align:middle;overflow:hidden;text-decoration:none;color:inherit;background-color:inherit;text-align:center;cursor:pointer;white-space:nowrap} .w3-btn:hover{box-shadow:0 8px 16px 0 rgba(0,0,0,0.2),0 6px 20px 0 rgba(0,0,0,0.19)} .w3-btn{-webkit-touch-callout:none;-webkit-user-select:none;-khtml-user-select:none;-moz-user-select:none;-ms-user-select:none;user-select:none} .w3-disabled,.w3-btn:disabled{cursor:not-allowed;opacity:0.3}:disabled *{pointer-events:none} .w3-btn.w3-disabled:hover,.w3-btn:disabled:hover{box-shadow:none} .w3-input{padding:8px;display:block;border:none;border-bottom:1px solid #ccc;width:100%} .w3-check{width:24px;height:24px;position:relative;top:6px} .w3-container:after,.w3-container:before,.w3-panel:after,.w3-panel:before{content:\"\";display:table;clear:both} .w3-content{margin-left:auto;margin-right:auto}.w3-content{max-width:980px}.w3-round{border-radius:4px} .w3-container,.w3-panel{padding:0.01em 16px}.w3-panel{margin-top:16px;margin-bottom:16px} .w3-card{box-shadow:0 2px 5px 0 rgba(0,0,0,0.16),0 2px 10px 0 rgba(0,0,0,0.12)}.w3-center{text-align:center!important} .w3-green{color:#fff!important;background-color:#4CAF50!important} .w3-red{color:#fff!important;background-color:#f44336!important} .w3-teal{color:#fff!important;background-color:#009688!important} .w3-yellow{color:#000!important;background-color:#ffeb3b!important} .w3-dark-gray{color:#fff!important;background-color:#616161!important}</style><header class=\"w3-teal w3-container\"><h1 id=deviceName>Remote Switch</h1></header><div class=w3-content style=max-width:800px><div class=w3-container><div class=\"w3-card w3-panel\"id=switchCard><p class=w3-center><button class=\"w3-btn w3-disabled w3-round\"id=switchButton style=width:150px;height:150px;font-size:4rem>💡</button><p class=w3-center>Last update: <i id=lastUpdate>unknown</i></div><p class=w3-center><button class=\"w3-btn w3-teal\"id=showSettingsButton>Show settings</button><div class=\"w3-card w3-panel\"id=settings style=display:none><form action=password method=POST><p><input class=w3-input id=newPassword name=password required maxlength=25 minlength=8 type=password> <label>New password</label><p class=w3-center><button class=\"w3-btn w3-teal\"type=submit>Save</button></form><form action=deviceName method=POST><p><input class=w3-input id=newDeviceName name=deviceName required maxlength=25> <label>New device name</label><p class=w3-center><button class=\"w3-btn w3-teal\"type=submit>Save</button></form><form action=ipConfig method=POST><p><input class=w3-check id=enableDhcp name=dhcp type=checkbox> <label>Enable DHCP</label><p><input class=w3-input id=staticIP name=staticIP required> <label>IP address</label><p><input class=w3-input id=subnetMask name=subnetMask required> <label>Subnet mask</label><p><input class=w3-input id=gatewayIP name=gatewayIP> <label>Default gateway</label><p class=w3-center><button class=\"w3-btn w3-teal\"type=submit>Save</button></form><form action=reset method=POST onsubmit='return confirm(\"This will reset Remote Switch. Do you want to proceed?\")'><p class=w3-center><button class=\"w3-btn w3-yellow\"type=submit>Reset</button></form><form action=hardReset method=POST onsubmit='return confirm(\"This will reset all configuration settings to their default values. Do you want to proceed?\")'><p class=w3-center><button class=\"w3-btn w3-red\"type=submit>Restore configuration</button></form></div></div></div><script>!async function(t){var e=t.getElementById(\"deviceName\"),a=t.getElementById(\"switchButton\"),s=t.getElementById(\"lastUpdate\"),n=t.getElementById(\"showSettingsButton\"),d=t.getElementById(\"settings\"),i=t.getElementById(\"enableDhcp\"),c=t.getElementById(\"staticIP\"),l=t.getElementById(\"subnetMask\"),o=t.getElementById(\"gatewayIP\");var u=await async function(){return await(await fetch(\"status\",{mode:\"same-origin\"})).json()}();function g(){a.classList.remove(\"w3-disabled\"),a.classList.add(u.switchStatus?\"w3-green\":\"w3-dark-gray\"),s.textContent=(new Date).toUTCString()}e.textContent=u.deviceName,g();a.addEventListener(\"click\",async function(){var t=u.switchStatus?0:1;a.classList.remove(\"w3-green\",\"w3-dark-gray\"),a.classList.add(\"w3-disabled\"),200===(await fetch(\"switchStatus\",{method:\"POST\",mode:\"same-origin\",body:\"toggle=\"+t})).status&&(u.switchStatus=t),g()}),n.addEventListener(\"click\",function(){this.style.display=\"none\",d.style.display=\"block\"});n=\"^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$\";function r(){c.disabled=i.checked,c.value=\"\",l.disabled=i.checked,l.value=\"\",o.disabled=i.checked,o.value=\"\"}c.pattern=n,l.pattern=n,o.pattern=n,i.checked=!u.staticIP,r(),u.staticIP&&(c.value=u.staticIP,l.value=u.subnetMask,o.value=u.gatewayIP),i.addEventListener(\"change\",r)}(document);</script>";

void WebApp::wsDefaultCommand(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return false;
  }

  if (type != WebServer::GET) {
    server.httpFail();
    return;
  }

  server.httpSuccess();

  server.printP(WS_DEFAULT_COMMAND);
}

P(WS_STATUS_PART1) = "{\"deviceName\":\"";
P(WS_STATUS_PART2) = "\",\"switchStatus\":";
P(WS_STATUS_PART3) = ",\"staticIP\":\"";
P(WS_STATUS_PART4) = "\",\"subnetMask\":\"";
P(WS_STATUS_PART5) = "\",\"gatewayIP\":\"";
P(WS_STATUS_PART6) = "\"}";

void WebApp::wsStatus(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::GET) {
    server.httpFail();
    return;
  }

  server.httpSuccess("application/json");

  char ipString[16];

  server.printP(WS_STATUS_PART1);
  server.print(Config::getDeviceName());
  
  server.printP(WS_STATUS_PART2);
  server.print(Switch::getStatus());

  server.printP(WS_STATUS_PART3);
  if (Config::hasStaticIP()) {
    ip2string(Config::getStaticIP(), ipString);
    server.print(ipString);
  }
  
  server.printP(WS_STATUS_PART4);
  if (Config::hasStaticIP()) {
    ip2string(Config::getSubnetMask(), ipString);
    server.print(ipString);
  }
  
  server.printP(WS_STATUS_PART5);
  if (Config::hasGatewayIP()) {
    ip2string(Config::getGatewayIP(), ipString);
    server.print(ipString);
  }
  
  server.printP(WS_STATUS_PART6);
}

void WebApp::wsSwitchStatus(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::POST) {
    server.httpFail();
    return;
  }

  char pKey[32], pVal[32];
  
  while (server.readPOSTparam(pKey, sizeof(pKey), pVal, sizeof(pVal))) {
    if (pKey[sizeof(pKey) - 2] != 0 || pVal[sizeof(pVal) - 2] != '\0') {
      httpSeeOtherFail(server);
      return;
    }
    if (strcmp(pKey, "toggle") == 0) {
      if (strcmp(pVal, "0") == 0) {
        Switch::toggle(0);
        server.httpSuccess();
        return;
      } else if (strcmp(pVal, "1") == 0) {
        Switch::toggle(1);
        server.httpSuccess();
        return;
      }
    }
  }

  server.httpFail();
}

void WebApp::wsPassword(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::POST) {
    server.httpFail();
    return;
  }

  char pKey[32], pVal[32];
  
  while (server.readPOSTparam(pKey, sizeof(pKey), pVal, sizeof(pVal))) {
    if (pKey[sizeof(pKey) - 2] != 0 || pVal[sizeof(pVal) - 2] != '\0') {
      httpSeeOtherFail(server);
      return;
    }
    if (strcmp(pKey, "password") == 0) {
      if (Config::setPassword(pVal)) {
        Config::save();
        httpSeeOtherSuccess(server);
        return;
      }
    }
  }

  httpSeeOtherFail(server);
}

void WebApp::wsDeviceName(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::POST) {
    server.httpFail();
    return;
  }

  char pKey[32], pVal[32];

  while (server.readPOSTparam(pKey, sizeof(pKey), pVal, sizeof(pVal))) {
    if (pKey[sizeof(pKey) - 2] != 0 || pVal[sizeof(pVal) - 2] != '\0') {
      httpSeeOtherFail(server);
      return;
    }
    if (strcmp(pKey, "deviceName") == 0) {
      if (Config::setDeviceName(pVal)) {
        Config::save();
        httpSeeOtherSuccess(server);
        return;
      }
    }
  }

  httpSeeOtherFail(server);
}

void WebApp::wsIpConfig(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::POST) {
    server.httpFail();
    return;
  }

  uint32_t ip = 0, subnet = 0, gateway = 0;
  char pKey[32], pVal[32];

  while (server.readPOSTparam(pKey, sizeof(pKey), pVal, sizeof(pVal))) {
    if (pKey[sizeof(pKey) - 2] != 0 || pVal[sizeof(pVal) - 2] != '\0') {
      httpSeeOtherFail(server);
      return;
    }
    if (strcmp(pKey, "dhcp") == 0) {
      if (strcmp(pVal, "on") == 0) {
        Config::enableDhcp();
        Config::save();
        httpSeeOtherSuccess(server);
        return;
      }
    } else if (strcmp(pKey, "staticIP") == 0) {
      ip = string2ip(pVal);
    } else if (strcmp(pKey, "subnetMask") == 0) {
      subnet = string2ip(pVal);
    } else if (strcmp(pKey, "gatewayIP") == 0) {
      gateway = string2ip(pVal);
    }
  }

  if (gateway == 0) {
    if (Config::enableStaticIP(ip, subnet)) {
      Config::save();
      requestingReset = true;
      httpSeeOtherSuccess(server);
      return;
    }
  } else {
    if (Config::enableStaticIP(ip, subnet, gateway)) {
      Config::save();
      requestingReset = true;
      httpSeeOtherSuccess(server);
      return;
    }
  }

  httpSeeOtherFail(server);
}

P(WS_REDIRECT_PART1) =
  "<html>"
  "<head>"
  "<title>Remote Switch</title>"
  "<meta http-equiv=\"refresh\" content=\"5;url=/\" />"
  "</head>"
  "<body>"
  "<h1>";

P(WS_REDIRECT_PART2) =
  "</h1>"
  "<p>Redirecting in 5 seconds, please wait...</p>"
  "</body>"
  "</html>";

P(WS_REDIRECT_SUCCESS) = "Success!";

P(WS_REDIRECT_FAIL) = "Fail!";

void WebApp::wsRedirect(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::GET) {
    server.httpFail();
    return;
  }

  if (lastStatus == 0) {
    server.httpSeeOther("/");
    return;
  }
  
  server.httpSuccess();

  server.printP(WS_REDIRECT_PART1);
  server.printP(lastStatus == 1 ? WS_REDIRECT_SUCCESS : WS_REDIRECT_FAIL);
  server.printP(WS_REDIRECT_PART2);

  lastStatus = 0;

  if (requestingReset) {
    resetRequired = true;
  }
}

void WebApp::wsReset(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::POST) {
    server.httpFail();
    return;
  }

  requestingReset = true;

  httpSeeOtherSuccess(server);
}

void WebApp::wsHardReset(WebServer &server, WebServer::ConnectionType type, char *url_tail, bool tail_complete) {
  if (!server.checkCredentials(Config::getBasicCredentials())) {
    server.httpUnauthorized();
    return;
  }

  if (type != WebServer::POST) {
    server.httpFail();
    return;
  }

  Config::invalidate();
  requestingReset = true;

  httpSeeOtherSuccess(server);
}

int WebApp::lastStatus = 0;

bool WebApp::requestingReset = false;

bool WebApp::resetRequired = false;
