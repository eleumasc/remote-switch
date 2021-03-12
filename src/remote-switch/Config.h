#ifndef CONFIG_H_
#define CONFIG_H_

#include <Arduino.h>

#define network_address(ip, subnet) (ip & subnet)
#define broadcast_address(ip, subnet) (ip | ~subnet)

union ConfigData {
  struct {
    uint16_t checksum;
    char basicCredentials[45];
    char deviceName[26];
    byte netFlags;
    uint32_t staticIP;
    uint32_t subnetMask;
    uint32_t gatewayIP;
  } attr;
  byte rawBytes[86];
};

class Config {

  static ConfigData data;

  static char encodeCharBase64(char thisChar);
  
  static void encodeBase64(char* dest, const char* source);

  static uint32_t reverseByteOrder(uint32_t x);
  
  Config() {}
  
public:

  static const char* getBasicCredentials();

  static bool setPassword(const char* password);

  static const char* getDeviceName();

  static bool setDeviceName(const char* deviceName);

  static bool hasStaticIP();

  static uint32_t getStaticIP();

  static uint32_t getSubnetMask();

  static bool hasGatewayIP();

  static uint32_t getGatewayIP();

  static bool enableDhcp();

  static bool enableStaticIP(const uint32_t ipLong, const uint32_t subnetLong, const uint32_t gatewayLong = 0);

  static bool validate();

  static bool validateOnline();

  static void load();

  static void save();

  static void invalidate();

};

#endif
