#define WIND_SPD_PIN 14
volatile unsigned long timeSinceLastTick = 0;
volatile unsigned long lastTick = 0;    
float windSpeed;
long secsClock = 0;
void setup() {
     Serial.begin(115200);
     delay(25);
     Serial.println("\nWeather station powered on.\n");
     // Wind speed sensor setup. The windspeed is calculated according to the number
     //  of ticks per second. Timestamps are captured in the interrupt, and then converted
     //  into mph. 
     pinMode(WIND_SPD_PIN, INPUT);     // Wind speed sensor
     attachInterrupt(digitalPinToInterrupt(WIND_SPD_PIN), windTick, FALLING);
    

}

void loop() {
  Read_Sensors_Data();
  Serial.print("Windspeed: "); Serial.print(windSpeed*2.4); Serial.println(" mph");
  delay (1000);
}

 void Read_Sensors_Data()
{

// Read Weather Meters Datas ( Wind Speed, Rain Fall and Wind Direction )

      static unsigned long outLoopTimer = 0;
      static unsigned long wundergroundUpdateTimer = 0;
      static unsigned long clockTimer = 0;
      static unsigned long tempMSClock = 0;
    
      // Create a seconds clock based on the millis() count. We use this
      //  to track rainfall by the second. We've done this because the millis()
      //  count overflows eventually, in a way that makes tracking time stamps
      //  very difficult.
      tempMSClock += millis() - clockTimer;
      clockTimer = millis();
      while (tempMSClock >= 1000)
      {
        secsClock++;
        tempMSClock -= 1000;
      }
      
      // This is a once-per-second timer that calculates and prints off various
      //  values from the sensors attached to the system.
      if (millis() - outLoopTimer >= 2000)
      {
        outLoopTimer = millis();
        // Windspeed calculation, in mph. timeSinceLastTick gets updated by an
        //  interrupt when ticks come in from the wind speed sensor.
        if (timeSinceLastTick != 0) windSpeed = 1000.0/timeSinceLastTick;

       // Calculate the wind direction and display it as a string.

   
      }  
   }

// Keep track of when the last tick came in on the wind sensor.
    void windTick(void)
    {
      timeSinceLastTick = millis() - lastTick;
      lastTick = millis();
    }
