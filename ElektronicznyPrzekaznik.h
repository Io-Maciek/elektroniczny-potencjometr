class ElektronicznyPrzekaznik {
private:
  bool state;
  byte relayLevel;
  byte* pins;
  byte pinsLen;
  bool offState;

public:
  ElektronicznyPrzekaznik(byte gpioPins[], byte pinsArrayLength, bool defaultOffState){
    // zapisz piny, ustaw na output i włącz
    pinsLen = pinsArrayLength;
    offState=defaultOffState;

    pins = new byte[pinsLen]; // Alokuj pamięć dla pins
    memcpy(pins, gpioPins, pinsLen * sizeof(byte)); // Skopiuj gpioPins do pins
    
    
    for (int i =0; i < pinsLen; i++){
      pinMode(pins[i], OUTPUT);
      digitalWrite(pins[i], offState);
    }

    state = false;
    relayLevel=0;
  }

  bool setState(bool newState){
    state=newState;
    for (int i = 0; i < pinsLen; i++){
        // Ustawienie stanu i-tego pinu (pins[i]) na wysoki lub niski w zależności od poziomu sterowania (relayLevel).
        // Jeśli i-ty bit w relayLevel (w reprezentacji binarnej) jest ustawiony na 1, pin zostanie ustawiony na stan wysoki (ON).
        // W przeciwnym przypadku, pin zostanie ustawiony na wartość określoną przez offState (stan niski lub wyłączony).
      digitalWrite(pins[i], state?((relayLevel & (1 << i))):offState); // todo: zamiast 1 dać "!offState" | digitalWrite(pins[i], state?(((!offState)& (1 << i))):offState);
    }
    return true;
  }

  // void setRelayLevel(byte newRelayLevel){
  //       byte newR = map(powerLevel, 0, 100, 0, 15);
  //       relayLevel=newRelayLevel;
  //       for (int i =0; i < pinsLen; i++){
  //         digitalWrite(pins[i], (relayLevel & (1 << i))); // tu też: digitalWrite(pins[i], (relayLevel & ((!offState) << i)));
  //       }
  // }
  bool setRelayLevel(int &powerLevel){
    relayLevel = map(powerLevel, 0, 100, 0, pow(2, pinsLen) - 1);
    for (int i =0; i < pinsLen; i++){
      Serial.println(pins[i]);
      digitalWrite(pins[i], (relayLevel & (1 << i))); // tu też: digitalWrite(pins[i], (relayLevel & ((!offState) << i)));
    }
    Serial.println(relayLevel);
    return true;
  }

  byte getRelayLevel(){
    return relayLevel;
  }

  ~ElektronicznyPrzekaznik() {
    delete[] pins; // Zwolnij zaalokowaną pamięć
  }
};
