#include <Arduino.h>

#define mVperAmp 100
#define ACSoffset  2500

class ACS712{
  private:
   int pin;
  public:
   double voltage;
   double ampere;
   int raw_value;

    ACS712(int analogPin){
      pin=analogPin;
      voltage = 0.0;
      ampere=0.0;
      raw_value=0;
    }

    void Measure(){
      raw_value = analogRead(pin);
      voltage = (raw_value / 1024.0) * 5000; // Gets you mV
      ampere = ((voltage - ACSoffset) / mVperAmp);
    }

    void Print(){
      Serial.print("Raw Value = " ); // shows pre-scaled value
      Serial.print(raw_value);
      Serial.print("\t mV = "); // shows the voltage measured
      Serial.print(voltage,3); // the '3' after voltage allows you to

      Serial.print("\t Amps = "); // shows the voltage measured
      Serial.println(ampere,3); // the '3' after voltage allows you to display
    }

  String json() {
    String jsonStr = "{";
    jsonStr += "\"voltage\": " + String(voltage, 3) + ",";
    jsonStr += "\"ampere\": " + String(ampere, 3) + ",";
    jsonStr += "\"raw_value\": " + String(raw_value);
    jsonStr += "}";
    return jsonStr;
  }
};