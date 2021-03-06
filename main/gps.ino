TinyGPS gps;
char buff[32];
NeoSWSerial ss(GPS_RXPin, GPS_TXPin);

void gpsStart() {
  ss.begin(GPSBaud);
  delay(100);
  ss.read();
}

void getCoordinates(char *&coordinates)
{  
  gpsInitPins();
  delay(100);
  Serial.print("Attempting to wake GPS module.. ");
  gpsOn();
  Serial.println("done.");
  delay(100);
  Serial.println("Getting coordinates");
  delay(100);
  float la, lo;
  unsigned long age;
  float prec = TinyGPS::GPS_INVALID_HDOP;
  delay(100);
  while (prec == TinyGPS::GPS_INVALID_HDOP | prec > 4)
  {
    bool newData = false;
    unsigned long chars;
    unsigned short sentences, failed;
    for (unsigned long start = millis(); millis() - start < 1000;)
    {
      while (ss.available())
      {
        char c = ss.read();
        if (gps.encode(c))
          newData = true;
      }
    }

    if (newData)
    {
      gps.f_get_position(&la, &lo, &age);
      delay(1000);      
      prec = gps.hdop() / 100;
    }
  }
  Serial.println("Got fix");
  char slat[10];
  char slon[10];
  dtostrf(la, 4, 6, slat);
  dtostrf(lo, 4, 6, slon);
  snprintf(coordinates, 40, "{\"lat\": %s, \"long\": %s}", slat, slon);
  delay(100);
  Serial.println("Turning off GPS");
}

void gpsStop() {
  ss.end();
}
