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

  void setState(bool newState){
      state=newState;
      for (int i =0; i < pinsLen; i++){
        digitalWrite(pins[i], state?((relayLevel & (1 << i))):offState);
      }  
  }

  void setRelayLevel(byte newRelayLevel){
        relayLevel=newRelayLevel;
        for (int i =0; i < pinsLen; i++){
          digitalWrite(pins[i], (relayLevel & (1 << i)));
        }
  }

  ~ElektronicznyPrzekaznik() {
    delete[] pins; // Zwolnij zaalokowaną pamięć
  }
};
