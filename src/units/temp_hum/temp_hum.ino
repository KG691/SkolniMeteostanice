#include <GyverHTU21D.h>
GyverHTU21D htu;

void setup() {
  Serial.begin(9600);
  if (!htu.begin()) Serial.println(F("HTU21D error"));
}

void loop() { 
  htu.requestTemperature();
  delay(100);                                                
  if (htu.readTemperature()) {              
    Serial.print("Teplota: "); Serial.print(htu.getTemperature()); Serial.println(" °C");
  }
 
  htu.requestHumidity();                   
  delay(100);                              
  if (htu.readHumidity()) {                
    Serial.print("Vlhkost vzduchu: "); Serial.print(htu.getHumidity()); Serial.println(" %");
  }
  delay(500);
}
