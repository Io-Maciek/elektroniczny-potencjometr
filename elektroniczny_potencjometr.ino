#include <ESP8266WiFi.h>
#include "ElektronicznyPrzekaznik.h"
#include "WifiConfig.h"
#include "AccessPoint.h"


#include "SinricPro.h"
#include "SinricProDimSwitch.h"

#define WIFI_SSID         "<SSID HERE>"
#define WIFI_PASS         "<WIFI PASSWORD HERE>"
#define APP_KEY           "<APP KEY HERE>"
#define APP_SECRET        "<APP SECRET HERE>"
#define DEVICE_ID         "<DEVICE ID HERE>"


byte pins[] = {0,1,2,3};
ElektronicznyPrzekaznik relay(pins, 4, LOW);
SinricProDimSwitch &myDimSwitch = SinricPro[DEVICE_ID];
AccessPoint server;

bool onPowerState(const String &deviceId, bool &state) {
  relay.setState(state);
  return true; // request handled properly
}

bool onPowerLevel(const String &deviceId, int &powerLevel) {
  relay.setRelayLevel(map(powerLevel, 0, 100, 0, 15));
  return true;
}

bool onAdjustPowerLevel(const String &deviceId, int &levelDelta) {
  return true;
}

void setup() {
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
