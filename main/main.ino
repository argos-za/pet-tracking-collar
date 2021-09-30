#include <Arduino.h>
#include "TinyGPS.h"
#include "GPS.h"
#include <NeoSWSerial.h>
#include <Adafruit_FONA.h>

unsigned long startTime;
unsigned long endTime;
unsigned long frequency = 300000;
char *payload = new char[255]();
char *coordinates = new char[40]();

void setup()
{
    Serial.begin(115200);
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
    delay(1000);    
    gsmStart();
    delay(1000);
    generatePayload(coordinates, payload);
    delay(1000);
    delay(1000);
    sendPayload(payload);
    gsmStop();
}

void generatePayload(char *coordinates, char *&payload)
{
    strcpy(payload, "{\"sn\":\"argos1\",");
    sprintf(behind(payload), "%s", coordinates);
    sprintf(behind(payload), ", \"batt\": %d}", getBatteryPercentage());
}
