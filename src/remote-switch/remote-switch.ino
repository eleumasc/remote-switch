#include <Arduino.h>
#include "Switch.h"
#include "Config.h"
#include <SPI.h>
#include <Ethernet.h>
#include "WebServer.h"
#include "WebApp.h"

const void (*__reset)(void) = 0;

byte MAC_ADDRESS[6] = { 0x90, 0xA2, 0xDA, 0x8B, 0x54, 0x61 };

WebServer ws("", 80);

void setup() {
  Serial.begin(9600);

  Switch::init(9);

  if (Config::validateOnline()) {
    Config::load();
    if (!Config::validate()) {
      while (true) delay(1);
    }
  } else {
    if (!Config::validate()) {
      while (true) delay(1);
    }
    Config::save();
  }

  if (Config::hasStaticIP()) {
    Ethernet.begin(MAC_ADDRESS, IPAddress(Config::getStaticIP()));
    Ethernet.setSubnetMask(IPAddress(Config::getSubnetMask()));
  } else {
    Ethernet.begin(MAC_ADDRESS);
  }

  if (Config::hasGatewayIP()) {
    Ethernet.setGatewayIP(IPAddress(Config::getGatewayIP()));
  }

  WebApp::addAllCommands(ws);

  ws.begin();
}

void loop() {
  ws.processConnection();

  if (WebApp::isResetRequired()) {
    __reset();
  }
  
  Ethernet.maintain();
}
