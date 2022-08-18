#define RAIN_PIN        25
#define S_V_H           3600
#define NO_RAIN_SAMPLES 2000     

volatile long rainTickList[NO_RAIN_SAMPLES];
volatile int rainTickIndex = 0;
volatile int rainTicks = 0;
int rainLastDay = 0;
int rainLastHour = 0;
int rainLastHourStart = 0;
int rainLastDayStart = 0;
long secsClock = 0;

void setup() {
  pinMode(RAIN_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(RAIN_PIN), rainTick, FALLING);
      
  for (int i = 0; i < NO_RAIN_SAMPLES; i++) rainTickList[i] = 0;   
}

void loop() {
  Read_Sensors_Data();
  Serial.print("Srážky za poslední hodinu: "); Serial.println(float(rainLastHour) * 0.011, 3);
  delay(100);
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
    rainLastHour = 0;
    rainLastDay = 0;
    
    if (rainTicks > 0) {
      int i = rainTickIndex-1;
      while ((rainTickList[i] >= secsClock - S_V_H) && rainTickList[i] != 0) {
        i--;
        if (i < 0) i = NO_RAIN_SAMPLES-1;
        rainLastHour++;
      }
    }
  }  
}

void rainTick() {
  rainTickList[rainTickIndex++] = secsClock;
  if (rainTickIndex == NO_RAIN_SAMPLES) rainTickIndex = 0;
  rainTicks++;
}
