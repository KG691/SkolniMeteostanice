#include <WiFi.h>
#include <Wire.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>

char pressure_value[4];

#define DHTPIN 15 
#define DHTTYPE DHT22
// Init teplomeru
DHT dht(DHTPIN, DHTTYPE);
// Init tlakoveho cidla
Adafruit_BMP085 bmp;

const char* wifi_name = "UPC1306669_EXT";    //Vyměnit za wifi školy
const char* wifi_pass = "5ywUubdjuepw";  //Vyměnit za heslo na wifi školy
WiFiServer server(80);                   //Port 80

// spustí se s nastartováním:
void setup() {
  // rychlost 9600 bitů za sekundu:
  Serial.begin(9600);
  dht.begin();
  bmp.begin();

  // připojí se k vybrané wifi
  Serial.print("Connecting to ");
  Serial.print(wifi_name);
  WiFi.begin(wifi_name, wifi_pass);   
//dokavaď se nepřipojí tak se bude pokoušet připojit a nastartovat server, pokud se připojí tak se ten loop přeruší
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("Connection Successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());   //Dostaneme adresu našeho web serveru
  Serial.println("Type the above IP address into a browser search bar");
  server.begin();                   //Nastartujeme server
}

// nekonečný loop snímání z čidel:
void loop() {
  String pressure = String(bmp.readPressure());
  // convert the reading to a char array
  pressure.toCharArray(pressure_value, 4);
  
  float hum = dht.readHumidity(); 
  float temp = dht.readTemperature(); 
  
  WiFiClient client = server.available();
  if (client) {                             
    Serial.println("new client");          
    String currentLine = "";                   //uložíme si přijmutá data do stringu
    while (client.connected()) {            
      if (client.available()) {                
        char c = client.read();                // přečteme byte
        if (c == '\n') {                     
          if (currentLine.length() == 0) {     //v html můžeme upravovat vzhled naší stránky
            client.print("<html><head><title> ESP32 Weather Station</title>");
            client.print("<meta http-equiv='refresh' content='1'></head>");
            client.print("<body bgcolor=\"#E6E6FA\"><h1 style=\"text-align: center; color: blue\"> ESP32 Weather Station </h1>");
            client.print("<p style=\"text-align: center; font-size:150% \">Temperature in: ");
            client.print(temp);
            client.print("&deg;C<br/>");
            client.print("Humidity is: ");
            client.print(hum);
            client.print("%<br/>");
            client.print("Pressure is: ");
            client.print(pressure_value);
            client.print("hpa");
            client.print("</p></body>");
        
            break;  // break out of the while loop:
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
         } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;       // add it to the end of the currentLine
         }
        }
      }
    }

//         float temp = dht.readTemperature(); 
//         float hum = dht.readHumidity(); 
//         Serial.println(String("temp: ") + String(temp, 2) + String("C, hum: " + String(hum, 2)));
//
//         String pressure = String(bmp.readPressure());
//         pressure.toCharArray(pressure_value, 4);
//         Serial.println(String("press: ") + String(pressure_value) + String("hpa"));

  delay(500);        // pauza mezi čteními
}
