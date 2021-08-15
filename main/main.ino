#include <Arduino.h>
#include "TinyGPS.h"
#include "GPS.h"
#include <AltSoftSerial.h>
#include <NeoSWSerial.h>
#include <Adafruit_FONA.h>
#include <stdio.h>

unsigned long startTime;
unsigned long endTime;
unsigned long frequency = 300000;
char *payload = new char[255]();
char *response = new char[255]();
char *coordinates = new char[40]();

void setup()
{
    Serial.begin(115200);
}

void loop()
{
    startTime = millis();
    poll();
    endTime = millis();
    Serial.print(frequency - (endTime - startTime));
    delay(frequency - (endTime - startTime));
}

void poll()
{
    Serial.println("Starting GPS and getting coordinates");
    gpsStart();
    getCoordinates(coordinates);
    gpsStop();
    delay(1000);
    Serial.print("Coordinates: ");
    Serial.println(coordinates);        
    gsmStart();
    delay(1000);
    generatePayload(coordinates, payload);
    delay(100);
    Serial.print("Payload: ");
    Serial.println(payload);
    delay(100);
    sendPayload(payload);
    gsmStop();
    Serial.print("Done");
}

void generatePayload(char *coordinates, char *&payload)
{
    strcpy(payload, "{\"sn\":\"argos1\",");
    sprintf(behind(payload), "%s", coordinates);
    sprintf(behind(payload), ", \"batt\": %d}", getBatteryPercentage());
}
