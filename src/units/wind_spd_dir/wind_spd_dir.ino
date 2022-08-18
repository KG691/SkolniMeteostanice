#define WIND_SPD_PIN 14
#define WIND_DIR_PIN 35

volatile unsigned long timeSinceLastTick = 0;
volatile unsigned long lastTick = 0;    
float windSpeed;
int vin; 
String windDir = "";
long secsClock = 0;

void setup() {
  Serial.begin(115200);
  delay(25);
  Serial.println("\nWeather station powered on.\n");

  pinMode(WIND_SPD_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(WIND_SPD_PIN), windTick, FALLING);
}

void loop() {
  Read_Sensors_Data();
  Serial.print("Rychlost větru: "); Serial.print(windSpeed*2.4*1,6); Serial.println(" km/h");    
  Serial.print("Směr větru: ");  Serial.print("  "); Serial.println(windDir);  
  delay (500);
}

void Read_Sensors_Data() {



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
          
    if (timeSinceLastTick != 0) windSpeed = 1000.0/timeSinceLastTick;   
    windDirCalc();         
  } 
}

void windTick() {
  timeSinceLastTick = millis() - lastTick;
  lastTick = millis();
}

void windDirCalc() {
    pinMode(WIND_DIR_PIN, INPUT);  
    vin = analogRead(WIND_DIR_PIN);
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
