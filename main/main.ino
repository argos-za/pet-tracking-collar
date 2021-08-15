#include <Arduino.h>
#include <TinyGPS.h>
#include "GPS.h"
#include <AltSoftSerial.h>
#include <NeoSWSerial.h>
#include <Adafruit_FONA.h>

unsigned long startTime;
unsigned long endTime;
unsigned long frequency = 300000;
char *payload = new char[255]();
char *response = new char[255]();
char *coordinates = new char[100]();

void setup()
{
    Serial.begin(9600);
    gsmInit();
    gpsInit(); 
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
    getCoordinates(coordinates);
    Serial.print(F("Coordinates: "));
    Serial.print(coordinates);
    generatePayload(coordinates, payload);
    Serial.print(F("Payload: "));
    Serial.print(payload);
    gsmInit();
    turnOnGSM();    
    sendPayload(payload);
}

void generatePayload(char *coordinates, char *&payload)
{
    strcpy(payload, "{\"serial\":\"argos1\",");
    sprintf(behind(payload), "%s", coordinates);
    sprintf(behind(payload), ", \"battery\": %d}", getBatteryPercentage());
}
