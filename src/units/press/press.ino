#include <Adafruit_BMP085.h>
Adafruit_BMP085 bmp;
char pressure_value[4];

void setup() {
Serial.begin(115200);
  bmp.begin();
}
void loop() {
  Read_Sensors_Data();
  Serial.print("Tlak: "); Serial.print(pressure_value); Serial.println("hpa");  
  delay (2000);
}
void Read_Sensors_Data() {
  String pressure = String(bmp.readPressure());
  pressure.toCharArray(pressure_value, 4);
}    
