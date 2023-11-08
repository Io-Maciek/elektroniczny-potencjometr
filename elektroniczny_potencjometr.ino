#include <ESP8266WiFi.h>
#include "SinricPro.h"
#include "SinricProDimSwitch.h"

#include "ElektronicznyPrzekaznik.h"
#include "WifiConfig.h"
#include "AccessPoint.h"

#define APP_KEY           "<APP KEY HERE>"
#define APP_SECRET        "<APP SECRET HERE>"
#define DEVICE_ID         "<DEVICE ID HERE>"



byte pins[] = {0,1,2,3};
ElektronicznyPrzekaznik relay(pins, 4, LOW);
SinricProDimSwitch &myDimSwitch = SinricPro[DEVICE_ID];
AccessPoint server = AccessPoint(myDimSwitch, relay);

bool onPowerState(const String &deviceId, bool &state) {
  return relay.setState(state);
}

bool onPowerLevel(const String &deviceId, int &powerLevel) {
  //OLD -> relay.setRelayLevel(map(powerLevel, 0, 100, 0, 15)); // TODO 2^4 - 1 (4->długość tablicy *pins*) - dać ten fragment do metody setRelayLevel
  return relay.setRelayLevel(powerLevel);
}

bool onAdjustPowerLevel(const String &deviceId, int &levelDelta) {
  return true;
}

void setup() {
  WiFi.setAutoReconnect(true);
  WifiConfig c;
  WiFi.begin(c.getSSID(), c.getPWD());           // start wifi
  unsigned long startTime = millis();  // czas rozpoczęcia
  unsigned long timeout = 10000;       // czas oczekiwania (w milisekundach)
  bool connected = false;              

  while (millis() - startTime < timeout) {
    if (WiFi.status() == WL_CONNECTED) {
      connected = true;
      break;
    }
    delay(250);
  }
  
  myDimSwitch.onPowerState(onPowerState);
  myDimSwitch.onPowerLevel(onPowerLevel);
  myDimSwitch.onAdjustPowerLevel(onAdjustPowerLevel);

  // setup SinricPro
  SinricPro.onConnected([](){ }); 
  SinricPro.onDisconnected([](){ });
  SinricPro.begin(APP_KEY, APP_SECRET);
}

void loop() {
  SinricPro.handle();                         // handle SinricPro commands
  server.handle();
}

/*
TODO's:
  - GET request to szafa server for time of day (turn off no matter what after XX:XX PM)
  - Turn off access point server after X minutes to preserve power ( + add option to turn it off manually)
*/
