#include <EEPROM.h>

#define SSID_SIZE 32
#define PASSWORD_SIZE 32
#define SSID_ADDRESS 0
#define PASSWORD_ADDRESS 32


class WifiConfig{
public:  
  String getSSID() {
    EEPROM.begin(SSID_SIZE);
    char ssid[SSID_SIZE];
    int i;
    for (i = 0; i < SSID_SIZE; i++) {
      ssid[i] = EEPROM.read(SSID_ADDRESS + i);
      if (ssid[i] == '\0') {
        break;  // Znaleziono znak null - koniec SSID
      }
    }
    EEPROM.end();
    return String(ssid);
  }

  String getPWD(){
    EEPROM.begin(PASSWORD_SIZE+SSID_SIZE);
    char pwd[PASSWORD_SIZE];
    int i;
    for (i = 0; i < PASSWORD_SIZE; i++) {
      pwd[i] = EEPROM.read(PASSWORD_ADDRESS + i);
      if (pwd[i] == '\0') {
        break;  // Znaleziono znak null - koniec SSID
      }
    }
    EEPROM.end();
    return String(pwd);
  }

  void setSSID(char *ssid){
    int ssidLength = strlen(ssid);

    EEPROM.begin(SSID_SIZE);
    delay(50);

    // Zapisz SSID
    int address = SSID_ADDRESS;
    for (int i = 0; i < ssidLength; i++) {
      EEPROM.write(address + i, ssid[i]);
    }
    EEPROM.write(address + ssidLength, '\0');  // Znak null na końcu SSID
    EEPROM.commit();
    EEPROM.end();
  }
  
  void setPWD(char *password) {
    int passwordLength = strlen(password);

    EEPROM.begin(PASSWORD_SIZE+SSID_SIZE);
    delay(50);
    
    int address = PASSWORD_ADDRESS;
    for (int i = 0; i < passwordLength; i++) {
      EEPROM.write(address + i, password[i]);
    }
    EEPROM.write(address + passwordLength, '\0');  // Znak null na końcu hasła

    EEPROM.commit();
    EEPROM.end();
  }

};
