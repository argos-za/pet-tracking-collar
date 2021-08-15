#define FONA_RX 11
#define FONA_TX 10
#define FONA_RST 9

AltSoftSerial simss(FONA_TX, FONA_RX);
AltSoftSerial *fonaSerial = &simss;

Adafruit_FONA sim = Adafruit_FONA(FONA_RST);

uint8_t net_status;
boolean gprs_on = false;

char url[] = "api.thingspeak.com/update?api_key=OBMUL3QGC46S3T0L&status";

uint16_t statuscode;
int16_t length;

void gsmStart()
{  
  fonaSerial->begin(9600);
  //simss.println("AT+CSCLK=2");
}

void turnOnGSM()
{  
  while (sim.available()) {
    Serial.write(sim.read());

    while (net_status != 1)
  {
    Serial.println("Connecting to network");
    net_status = sim.getNetworkStatus();
    Serial.print(F("Network status "));
    Serial.print(net_status);
    Serial.print(F(": "));
    if (net_status == 0) Serial.println(F("Not registered"));
    if (net_status == 1) Serial.println(F("Registered (home)"));
    if (net_status == 2) Serial.println(F("Not registered (searching)"));
    if (net_status == 3) Serial.println(F("Denied"));
    if (net_status == 4) Serial.println(F("Unknown"));
    if (net_status == 5) Serial.println(F("Registered roaming"));
    delay(5000);
  }
  while (!gprs_on)
  {
    if (!sim.enableGPRS(true))
    {
      Serial.println("Failed to turn on GPRS");
      Serial.println("Trying again...");
      delay(2000);
      gprs_on = false;
    }
    else
    {
      Serial.println("GPRS now turned on");
      delay(2000);
      gprs_on = true;
      sim.setHTTPSRedirect(true);
    }
  }
  }
  
  
}

uint16_t getBatteryPercentage()
{
  uint16_t vbat;
  sim.getBattPercent(&vbat);
  return vbat;
}

void sendPayload(char *payload)
{
  Serial.print("Start payload send");
  if (!gprs_on)
  {
    turnOnGSM();
  }
  else
  {
    sim.HTTP_POST_start(url, F("text/plain"), (uint8_t *)payload, strlen(payload), &statuscode, (uint16_t *)&length);
    while (length > 0)
    {
      while (sim.available())
      {
        char c = sim.read();
      }
    }
    // parseCommands(c);
  }
}

// void parseCommands(char *commands){

// }

void gsmStop()
{  
  fonaSerial->end();
  //simss.println("AT+CSCLK=2");
}
