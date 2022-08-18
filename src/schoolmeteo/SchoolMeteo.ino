//libraries
#include <PubSubClient.h> //library for MQTT
#include <WiFi.h>  
#include <Wire.h>
#include <DHT.h>
#include <GyverHTU21D.h>
#include <Adafruit_BMP085.h>

//defining pins

#define DHTPIN 15 
#define DHTTYPE DHT22
#define WIND_SPD_PIN 14
#define RAIN_PIN     25
#define WIND_DIR_PIN 35

// Init of thermometer
GyverHTU21D htu;
// Init of barometric pressure sensor

Adafruit_BMP085 bmp;

//Variables used in the speed of wind calculations

volatile unsigned long timeSinceLastTick = 0;
volatile unsigned long lastTick = 0;    
float windSpeed;

// Variables for the direction of the wind

int vin; 
String windDir = "";

//Variables for the raincount
#define S_V_H       3600
#define NO_RAIN_SAMPLES 2000     
volatile long rainTickList[NO_RAIN_SAMPLES];
volatile int rainTickIndex = 0;
volatile int rainTicks = 0;
int rainLastHour = 0;
int rainLastHourStart = 0;
long secsClock = 0;

//MQTT Setup, use if you want MQQT
#define mqtt_server "192.168.0.59/"
  WiFiClient espClient;
  PubSubClient client(espClient);
  #define mqttHum "GJK/hum"
  #define mqttTemp "GJK/temp"
  #define mqttWndSpd "GJK/wndspd"
  #define mqttWndDir "GJK/wnddir"
  #define mqttPrss "GJK/prss"
  #define mqttRnfll "GJK/rnfll"
 

//Wi-fi connection
    
const char* wifi_name = "UPC1306669_EXT";    //Switch for the wanted wi-fi
const char* wifi_pass = "5ywUubdjuepw";  //Switch for the password for the wi-fi
WiFiServer server(80);

//Server variables

float temp = 0;
float hum = 0;
char pressure_value[4];

//Setup funcion

void setup() {
  Serial.begin(115200);
  delay(25);
  Serial.println("\nWeather station powered on.\n");

  //setup_mqtt(); //use setup_mqtt() for mqtt and setup_wifi() for wi-fi server
  setup_wifi();
      
  if (!htu.begin()) Serial.println(F("HTU21D error"));
    bmp.begin();

  //Setup the wind speed sensor
  pinMode(WIND_SPD_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(WIND_SPD_PIN), windTick, FALLING);  
  //Setup the rainfall sensor     
  pinMode(RAIN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), rainTick, FALLING);
      
  for (int i = 0; i < NO_RAIN_SAMPLES; i++) rainTickList[i] = 0;  
  bmp.begin();

}
void setup_wifi(){
 // Connect to wifi selected above
  
  Serial.print("Connecting to ");
  Serial.print(wifi_name);
  WiFi.begin(wifi_name, wifi_pass);   
        
  //Trying to connect until success
    
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
    
  Serial.println("");
  Serial.println("Connection Successful");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());   //Prints out the web server adress
  Serial.println("Type the above IP address into a browser search bar");
  
  server.begin();                   //Starts the adress
 
}

void setup_mqtt() {
   // begin Wifi connect
    Serial.print("Connecting to ");
    Serial.println(wifi_name);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(2000);
    WiFi.begin(wifi_name,wifi_pass );
    
   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
    }
 
    Serial.println("");
    Serial.println("WiFi connected");  
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    //end Wifi connect
 
    client.setServer(mqtt_server, 1883);
  
 }



//loop function

void loop() {
    Read_Sensors_Data();
    printdata();
    wifi_server();     //use wifi_server for posting the website in local network
//    publish_mqtt (); //use publish_mqtt for running mqtt
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
  // convert the reading to a char array
  pressure.toCharArray(pressure_value, 4);
  // Read the temperature and humidity
  
  // Read Wind Speed, Rain Fall and Wind Direction

  static unsigned long outLoopTimer = 0;
  static unsigned long wundergroundUpdateTimer = 0;
  static unsigned long clockTimer = 0;
  static unsigned long tempMSClock = 0;
      
  tempMSClock += millis() - clockTimer;
  clockTimer = millis();
  while (tempMSClock >= 1000) {
    secsClock++;
    tempMSClock -= 1000;
  }
  
  if (millis() - outLoopTimer >= 2000) {
    outLoopTimer = millis();
        
    // Windspeed calculation in mph. Times 1,6 for km/h
        
    if (timeSinceLastTick != 0) windSpeed = 1000.0/timeSinceLastTick;

    // Calculate the wind direction
    
    windDirCalc();       
     
    rainLastHour = 0;

    if (rainTicks > 0) {

      int i = rainTickIndex-1;
    
      // Counting the tics in the last hour
    
      while ((rainTickList[i] >= secsClock - S_V_H) && rainTickList[i] != 0) {
        i--;
        if (i < 0) i = NO_RAIN_SAMPLES-1;
        rainLastHour++;
      }
    
    }
  }  
}

// Times the tics of the sensor
    
void windTick() {
  timeSinceLastTick = millis() - lastTick;
  lastTick = millis();
}

void rainTick() {
  rainTickList[rainTickIndex++] = secsClock;
  if (rainTickIndex == NO_RAIN_SAMPLES) rainTickIndex = 0;
  rainTicks++;
}
    
// reading wind direction

void windDirCalc() {
    pinMode(WIND_DIR_PIN, INPUT);  
    vin = analogRead(WIND_DIR_PIN);
//table for the wind direction
    if (vin < 150) windDir = "Westnorthwest";
    else if (vin < 300) windDir = "West";
    else if (vin < 400) windDir = "Northnorthwest";
    else if (vin < 600) windDir = "Northwest";
    else if (vin < 900) windDir = "Northnortheast";
    else if (vin < 1100) windDir = "North";
    else if (vin < 1500) windDir = "Southsouthwest";
    else if (vin < 1700) windDir = "Southwest";
    else if (vin < 2250) windDir = "Eastnortheast";
    else if (vin < 2350) windDir = "Northeast";
    else if (vin < 2700) windDir = "Southsoutheast";
    else if (vin < 3000) windDir = "South";
    else if (vin < 3200) windDir = "Eastsoutheast";
    else if (vin < 3400) windDir = "Southeast";
    else if (vin < 4000) windDir = "East";
    else windDir = "East";
}

//print out data
  void printdata(){
   Serial.print("Rychlost větru: "); Serial.print(windSpeed*2.4*1,6); Serial.println(" km/h");     
   Serial.print("Směr větru: ");  Serial.print("  "); Serial.println(windDir);   
   Serial.print("Srážky za poslední hodinu: "); Serial.println(float(rainLastHour) * 0.011, 3);
   Serial.print("Tlak:");Serial.print(pressure_value); Serial.println("hpa");
   
  }

  void wifi_server(){
    WiFiClient client = server.available();
    if (client) {                             
      Serial.println("new client");          
      String currentLine = "";                  
      while (client.connected()) {            
       if (client.available()) {                
        char c = client.read();             
        if (c == '\n') {                     
          if (currentLine.length() == 0) {     //we can change the look of the site in html
            client.print("<html><head><title> GJK weather station </title>");
            client.print("<meta http-equiv'content-type' content='text/html; charset=utf-8'>");
            client.print("<meta http-equiv='refresh' content='1'></head>");
            client.print("<body bgcolor=\"#E6E6FA\"><h1 style=\"text-align: center; color: blue\"> ESP32 Weather Station </h1>");
            client.print("<p style=\"text-align: center; font-size:150% \">Temperature: ");
            client.print(htu.getTemperature());
            client.print("&deg;C<br/>");
            client.print("Humidity: ");
            client.print(htu.getHumidity());
            client.print("%<br/>");
            client.print("Pressure: ");
            client.print(pressure_value);
            client.print("hpa<br/>");  
            client.print("Wind direction: ");
            client.print(windDir);
            client.print("<br/>");
            client.print("Wind speed: ");
            client.print(windSpeed*2.4*1,6);
            client.print("km/h <br/>");
            client.print("Rainfall last hour: ");
            client.print(rainLastHour * 0.011, 3);
            client.print("ml ");
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
  }



 
 
    // Loop until we're reconnected
  void reconnect() {
    int counter = 0;
    while (!client.connected()) {
     if (counter==5){
       ESP.restart();
      }
     counter+=1;
      Serial.print("Connecting to MQTT...");
      // Attempt to connect
   
      if (client.connect("GJKmeteostanice")) {
      Serial.println("connected");
      } else {
        Serial.print("failed, rc=");
        Serial.print(client.state());
        Serial.println(" try again in 5 seconds");
       // Wait 5 seconds before retrying
        delay(5000);
      }
    }
  }
 
  void publish_mqtt() 
  {
   if (!client.connected()){
     reconnect();
   }
   
 
    client.publish(mqttHum, String(hum).c_str(),true);
    client.publish(mqttTemp, String(temp).c_str(),true);
    client.publish(mqttWndSpd, String(windSpeed*2.4).c_str(),true);
    client.publish(mqttWndDir, String(windDir).c_str(),true);
    client.publish(mqttPrss, String(pressure_value).c_str(),true);
    client.publish(mqttRnfll, String(rainLastHour * 0.011, 3).c_str(),true);
 
 
    delay(5000);
    }
