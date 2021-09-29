#define FONA_RX 11
#define FONA_TX 10
#define FONA_RST 9

NeoSWSerial simss(FONA_TX, FONA_RX);
Adafruit_FONA sim = Adafruit_FONA(FONA_RST);

uint8_t net_status;
boolean gprs_on = false;

char url[] = "api.thingspeak.com/update?api_key=OBMUL3QGC46S3T0L&status=";
char http_cmd[255];

uint16_t statuscode;
int16_t length;

void gsmStart()
{
  simss.begin(9600);

  if (! sim.begin(simss)) {
    Serial.println("Couldn't find SIM800L");
    while (1);
  }

  Serial.println("SIM800L is OK"); 
  delay(3000);
  net_status = sim.getNetworkStatus();
  while (net_status != 1)
  {
    net_status = sim.getNetworkStatus();
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

uint16_t getBatteryPercentage()
{
  uint16_t vbat;
  sim.getBattPercent(&vbat);
  return vbat;
}

void sendPayload(char *payload)
{  
  if (!gprs_on)
  {
    gsmStart();
  }
  else
  {
    Serial.print("Start payload send");
    strcpy(http_cmd, url);
    strcat(http_cmd, "=");
    strcat(http_cmd, payload);
    Serial.print(http_cmd);
    sim.HTTP_GET_start(http_cmd, &statuscode, (uint16_t *)&length);
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
  sim.enableGPRS(false);
  Serial.print("GPRS now turned off");
  simss.end();
  //simss.println("AT+CSCLK=2");
}
