#include "Config.h"
#include <EEPROM.h>

char Config::encodeCharBase64(char thisChar) {
  if (thisChar < 26) {
    return thisChar + 65;
  } else if (thisChar < 52) {
    return thisChar + 71;
  } else if (thisChar < 62) {
    return thisChar - 4;
  } else if (thisChar == 62) {
    return '+';
  } else {
    return '/';
  }
}

void Config::encodeBase64(char* dest, const char* source) {
  uint16_t buff;
  int s = 0, d = 0;
  for (; source[s]; ++s) {
    buff = (buff << 8) | ((uint16_t) source[s] & 0xff);
    switch (s % 3) {
    case 0:
      dest[d++] = encodeCharBase64((buff >> 2) & 0x3f);
      break;
    case 1:
      dest[d++] = encodeCharBase64((buff >> 4) & 0x3f);
      break;
    case 2:
      dest[d++] = encodeCharBase64((buff >> 6) & 0x3f);
      dest[d++] = encodeCharBase64(buff & 0x3f);
    }
  }
  switch (s % 3) {
  case 1:
    dest[d++] = encodeCharBase64((buff & 0x03) << 4);
    dest[d++] = '=';
    dest[d++] = '=';
    break;
  case 2:
    dest[d++] = encodeCharBase64((buff & 0x0f) << 2);
    dest[d++] = '=';
  }
  dest[d++] = '\0';
}

uint32_t Config::reverseByteOrder(uint32_t x) {
  return ((x & 0xff) << 24) | ((x & 0xff00) << 8) | ((x >> 8) & 0xff00) | ((x >> 24) & 0xff);
}

const char* Config::getBasicCredentials() {
  return data.attr.basicCredentials;
}

bool Config::setPassword(const char* password) {
  int length = strlen(password);
  if (length < 8 || length > 25) {
    return false;
  }
  
  for (int i = 0; password[i]; ++i) {
    char thisChar = password[i];
    if (thisChar < 0x20 || thisChar > 0x7e) {
      return false;
    }
  }

  data.attr.basicCredentials[0] = 'Y';
  data.attr.basicCredentials[1] = 'W';
  data.attr.basicCredentials[2] = 'R';
  data.attr.basicCredentials[3] = 't';
  data.attr.basicCredentials[4] = 'a';
  data.attr.basicCredentials[5] = 'W';
  data.attr.basicCredentials[6] = '4';
  data.attr.basicCredentials[7] = '6';
  encodeBase64(&data.attr.basicCredentials[8], password);
  return true;
}

const char* Config::getDeviceName() {
  return data.attr.deviceName;
}

bool Config::setDeviceName(const char* deviceName) {
  int length = strlen(deviceName);
  if (length < 1 || length > 25) {
    return false;
  }
  
  for (int i = 0; deviceName[i]; ++i) {
    char thisChar = deviceName[i];
    if ((thisChar < 'A' || thisChar > 'Z') &&
        (thisChar < 'a' || thisChar > 'z') &&
        (thisChar < '0' || thisChar > '9') &&
        thisChar != 0x20) {
      return false;
    }
  }

  strcpy(data.attr.deviceName, deviceName);
  return true;
}

bool Config::hasStaticIP() {
  return ((data.attr.netFlags & 1) != 0);
}

uint32_t Config::getStaticIP() {
  return data.attr.staticIP;
}

uint32_t Config::getSubnetMask() {
  return data.attr.subnetMask;
}

bool Config::hasGatewayIP() {
  return ((data.attr.netFlags & 2) != 0);
}

uint32_t Config::getGatewayIP() {
  return data.attr.gatewayIP;
}

bool Config::enableDhcp() {
  data.attr.netFlags = 0;
  return true;
}

bool Config::enableStaticIP(const uint32_t ipLong, const uint32_t subnetLong, const uint32_t gatewayLong = 0) {
  const uint32_t ip = reverseByteOrder(ipLong);
  const uint32_t subnet = reverseByteOrder(subnetLong);

  if (subnet == 0 || subnet == ~0) {
    return false;
  }
  const uint32_t prefix = ~(subnet - 1);
  if ((prefix & (prefix - 1)) != 0) {
    return false;
  }

  const uint32_t network = network_address(ip, subnet);
  const uint32_t broadcast = broadcast_address(ip, subnet);

  if ((broadcast & 0xff000000) != 0x0a000000 &&
      (broadcast & 0xfff00000) != 0xac100000 &&
      (broadcast & 0xffff0000) != 0xc0a80000) {
    return false;
  }
  if (ip == network || ip == broadcast) {
    return false;
  }

  const uint32_t gateway = reverseByteOrder(gatewayLong);

  if (gateway != 0) {
    if (network_address(gateway, subnet) != network) {
      return false;
    }

    if (gateway == network || gateway == broadcast) {
      return false;
    }
  }

  data.attr.staticIP = ipLong;
  data.attr.subnetMask = subnetLong;
  data.attr.netFlags = 1;
  if (gateway != 0) {
    data.attr.gatewayIP = gatewayLong;
    data.attr.netFlags = 3;
  }
  return true;
}

bool Config::validate() {
  uint16_t checksum = 0;
  for (int i = 2; i < sizeof(data); ++i) {
    checksum += (uint16_t) data.rawBytes[i];
  }
  return (checksum == data.attr.checksum);
}

bool Config::validateOnline() {
  uint16_t checksum = 0;
  for (int i = 2; i < sizeof(data); ++i) {
    checksum += (uint16_t) EEPROM.read(i);
  }
  uint16_t checksumOnline = ((uint16_t) EEPROM.read(1) & 0xff) << 8 | ((uint16_t) EEPROM.read(0) & 0xff);
  return (checksum == checksumOnline);
}

void Config::load() {
  for (int i = 0; i < sizeof(data); ++i) {
    data.rawBytes[i] = EEPROM.read(i);
  }
}

void Config::save() {
  uint16_t checksum = 0;
  for (int i = 2; i < sizeof(data); ++i) {
    EEPROM.write(i, data.rawBytes[i]);
    checksum += (uint16_t) data.rawBytes[i];
  }
  EEPROM.write(0, (byte) (checksum & 0xff));
  EEPROM.write(1, (byte) ((checksum >> 8) & 0xff));
}

void Config::invalidate() {
  EEPROM.write(0, ~EEPROM.read(0));
}


ConfigData Config::data = {
  .rawBytes = {
    0x77, 0x0f, 0x59, 0x57, 0x52, 0x74, 0x61, 0x57, 0x34, 0x36,
    0x63, 0x47, 0x46, 0x7a, 0x63, 0x33, 0x64, 0x76, 0x63, 0x6d,
    0x51, 0x3d, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x4d, 0x79, 0x20,
    0x73, 0x77, 0x69, 0x74, 0x63, 0x68, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x01, 0xc0, 0xa8, 0x01, 0xc8, 0xff, 0xff,
    0xff, 0x00, 0x00, 0x00, 0x00, 0x00
  }
};
