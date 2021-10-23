#include <Arduino.h>
#include "TinyGPS.h"
#include "GPS.h"
#include <NeoSWSerial.h>
#include <Adafruit_FONA.h>

unsigned long startTime;
unsigned long endTime;
unsigned long frequency = 300000;
char payload[110];
char *coordinates = new char[40]();
boolean validCoordinates = false;

void setup()
{
  Serial.begin(115200);
  Serial.println(F("Starting"));
  aes_init();
}

void loop()
{
  startTime = millis();
  poll();
  endTime = millis();
  delay(frequency - (endTime - startTime));
}

void poll()
{
  gpsStart();
  getCoordinates(coordinates);
  gpsStop();
  delay(1000);
  if(!validCoordinates){
    Serial.println(F("Could not get coordinates. Skipping this iteration"));
  } else {
    gsmStart();
    delay(1000);
    generatePayload(coordinates, payload);
    delay(1000);
    sendPayload(payload);
    gsmStop();
  }  
}

void generatePayload(char *coordinates, char *payload)
{
  char imei[16] = {0};
  getImei(imei);
  sprintf(payload, "{\"sn\":\"%s\",", imei);
  sprintf(behind(payload), "%s", coordinates);
  sprintf(behind(payload), ",\"batt\":%d}", getBatteryPercentage());
}
