#include <Adafruit_BMP085.h>
#include <GyverHTU21D.h>
Adafruit_BMP085 bmp;
GyverHTU21D htu;
char pressure_value[4];

void setup() {
  Serial.begin(115200);
  if (!htu.begin()) Serial.println(F("HTU21D error"));
  bmp.begin();
}
void loop() {
  Read_Sensors_Data();
  Serial.print("Tlak: "); Serial.print(pressure_value); Serial.println("hpa");
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
  delay (2000);
}
void Read_Sensors_Data() {
  String pressure = String(bmp.readPressure());
  pressure.toCharArray(pressure_value, 4);
  delay(100);
}
