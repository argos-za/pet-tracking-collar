#include "AESLib.h"

#define BAUD 9600

#define FONA_RX 11
#define FONA_TX 10
#define FONA_RST 9

NeoSWSerial simss(FONA_TX, FONA_RX);
Adafruit_FONA sim = Adafruit_FONA(FONA_RST);

uint8_t net_status;
boolean gprs_on = false;

char url[] = "http://102.132.200.59:7777/api/location";
uint16_t statuscode;
int16_t length;

AESLib aesLib;

unsigned char cleartext[68] = {0};
char ciphertext[128];

const byte aes_key[] = {0x61, 0x72, 0x67, 0x6f, 0x73, 0x69, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21};
byte aes_iv[] = {0x61, 0x6c, 0x77, 0x61, 0x79, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21, 0x21};

void aes_init()
{
  aesLib.gen_iv(aes_iv);
  aesLib.set_paddingmode((paddingMode)0);
}

uint16_t encrypt(char * msg, uint16_t msgLen, byte iv[])
{
  return aesLib.encrypt((byte*)msg, msgLen, (char*)ciphertext, aes_key, sizeof(aes_key), iv);
}

void gsmStart()
{
  simss.begin(BAUD);

  if (!sim.begin(simss))
  {
    while (1);
  }
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
      delay(2000);
      gprs_on = false;
    }
    else
    {
      delay(2000);
      gprs_on = true;
    }
  }
}

uint16_t getBatteryPercentage()
{
  uint16_t vbat;
  sim.getBattPercent(&vbat);
  return vbat;
}

void getImei(char *imei) {
  sim.getIMEI(imei);
}

byte enc_iv[] = {0x61, 0x6c, 0x77, 0x61, 0x79, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21, 0x21};
byte enc_iv_to[] = {0x61, 0x6c, 0x77, 0x61, 0x79, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21, 0x21};

void sendPayload(char *payload)
{
  memcpy(enc_iv, enc_iv_to, sizeof(enc_iv_to));
  uint16_t encLen = encrypt((char*)payload, 77, enc_iv);  
  delay(100);
  
  if (!sim.HTTP_POST_start(url, F("text/plain"), (uint8_t *)ciphertext, strlen(ciphertext), &statuscode, (uint16_t *)&length)) {
    Serial.println(F("Failed"));
  }
  else {
    sim.HTTP_POST_end();
  }
}

void gsmStop()
{
  sim.enableGPRS(false);
  gprs_on = false;
  simss.end();
}
