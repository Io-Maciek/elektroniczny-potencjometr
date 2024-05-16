#include <ESP8266WiFi.h>
#include "SinricPro.h"
#include "SinricProDimSwitch.h"
#include <ArduinoOTA.h>

#include "WifiConfig.cpp"
#include "ACS712.cpp"
#include "ElektronicznyPrzekaznik.cpp"
#include "AccessPoint.h"



//#define APP_KEY           "<APP KEY HERE>"
//#define APP_SECRET        "<APP SECRET HERE>"
//#define DEVICE_ID         "<DEVICE ID HERE>"
#define APP_KEY           "a7f56519-d208-4ec6-8bc9-b18369691339"
#define APP_SECRET        "dfc2fb86-cf53-42dc-8ec1-9b3fa9085c39-47f6d7ae-13cb-4f09-a0b0-6771e22a3d78"
#define DEVICE_ID         "64996c4f743f91207040bd06"



// dawniej 4 było 15 w tym samym miejscu '{15,13,12,14}'
byte pins[] = {4,13,12,14};
ElektronicznyPrzekaznik relay(pins, 4, LOW);
SinricProDimSwitch &myDimSwitch = SinricPro[DEVICE_ID];
ACS712 avs = ACS712(A0);
AccessPoint server = AccessPoint(myDimSwitch, relay, avs);


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

unsigned long print_pause;

void setup() {
  print_pause = millis();

  Serial.begin(9600);
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

  // setup over the wifi uploading
  ArduinoOTA.setHostname("ESP-Grzalka");

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_FS
      type = "filesystem";
    }

    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();


  Serial.println("OK.");
  Serial.println(c.getSSID());
}


void loop() {
  SinricPro.handle();                         // handle SinricPro commands
  server.handle();
  ArduinoOTA.handle();

  // if(millis()>=print_pause){
  //   avs.Measure();
  //   print_pause=millis() + 1000;
  //   avs.Print();
  // }
}

/*
TODO's:
  - GET request to szafa server for time of day (turn off no matter what after XX:XX PM)
  - Turn off access point server after X minutes to preserve power ( + add option to turn it off manually)
*/
