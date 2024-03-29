#include <ESP8266WebServer.h>


class AccessPoint{
  void _indexHandler() {
    WifiConfig c;
    String ssid = c.getSSID();
    String pwd = c.getPWD();

    String html = "<html><head><meta charset='utf-8'><meta name='viewport' content='width=device-width, initial-scale=1, shrink-to-fit=no'><title>Potencjometr Config</title></head><body><h1>Ustaw WiFi - Potencjometr</h1><form method='POST' action='/setWifi'><input name='ssid' value='"+ssid+"'><input name='pwd' value='"+pwd+"'><input type='submit'></form></body></html>";
    server.send(200, "text/html", html);    
  }

  void fromSetPowerLevel() {
    if (server.method() != HTTP_POST) {
      server.send(405, "text/plain", "Method Not Allowed");
    }else if(!server.hasArg("value")){
      server.send(400, "text/plain", "Parameter 'value' was not provided");
    } else {
      int value = server.arg("value").toInt();
      value = value > 100 ? 100 : (value < 0 ? 0 : value);
      device.sendPowerLevelEvent(value);
      ep.setRelayLevel(value);

      server.send(200, "text/plain", String(ep.getRelayLevel())); // Wysłanie odpowiedzi HTTP 302 (przekierowanie)
    }
  }

  void getPowerLevel() {
      server.send(200, "text/plain", String(ep.getRelayLevel()));       
  }

  void formSetWifi() {
    if (server.method() != HTTP_POST) {
      server.send(405, "text/plain", "Method Not Allowed");
    } else {
      WifiConfig c;
      String gotSsid = server.arg("ssid");
      String gotPwd = server.arg("pwd");
      char ssidBuffer[gotSsid.length() + 1];
      char pwdBuffer[gotPwd.length() + 1];
      gotSsid.toCharArray(ssidBuffer, sizeof(ssidBuffer));
      gotPwd.toCharArray(pwdBuffer, sizeof(pwdBuffer));
      c.setSSID(ssidBuffer);
      c.setPWD(pwdBuffer);
      server.sendHeader("Location", "/"); // Ustawienie nagłówka Location na nowy adres URL
      server.send(302, "text/plain", ""); // Wysłanie odpowiedzi HTTP 302 (przekierowanie)
    }
  }

  void getAcs() {
    acs.Measure();
    server.send(200, "application/json", String(acs.json())); // Wysłanie odpowiedzi HTTP 302 (przekierowanie)
  }

  void _notFoundHandler(){
      server.send(404, "text/html", "ERR 404");
  }
    
  private:
    ESP8266WebServer  server;
    SinricProDimSwitch& device;
    ElektronicznyPrzekaznik& ep;
    ACS712& acs;

  public:
    AccessPoint(SinricProDimSwitch& d, ElektronicznyPrzekaznik& e, ACS712& avs) : server(80), device(d), ep(e), acs(avs){
      WiFi.softAP("esp12_PotencjometrGrzałkiAP", "12345678");

      server.on("/", [this]() { _indexHandler(); });
      server.on("/setWifi", [this]() { formSetWifi(); });
      server.on("/set", [this]() { fromSetPowerLevel(); });
      server.on("/get", [this]() { getPowerLevel(); });
      server.on("/acs", [this]() { getAcs(); });
      server.onNotFound([this]() { _notFoundHandler(); });
      server.begin();
    }

    void handle(){
      server.handleClient();
    }


};
