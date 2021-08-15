TinyGPS gps;
char buf[32];

NeoSWSerial ss(GPS_RXPin, GPS_TXPin);

void gpsInit(){
  ss.begin(4800);
}
void getCoordinates(char *&coordinates)
{
  //Serial.println("Turning on GPS");  
  gpsInitPins();
  delay(2000);
  gpsOn();
  delay(2000);
  //while (ss.available())ss.read();
  //Serial.println("Getting coordinates");
  float la, lo;
  unsigned long age = TinyGPS::GPS_INVALID_DATE;
  unsigned long date = 0;
  while (age == TinyGPS::GPS_INVALID_AGE | age > 1000)
  {
    gps.f_get_position(&la, &lo, &age);
  }
  strcpy(coordinates, "{\"lat\":");
  sprintf(behind(coordinates), "%f", la);
  sprintf(behind(coordinates), ",\"long\":");
  sprintf(behind(coordinates), "%f", lo);
  sprintf(behind(coordinates), "}");
  //Serial.println("Turning off GPS");
  gpsOff();
}
