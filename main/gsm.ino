#include "AESLib.h"

#define BAUD 9600

#define FONA_RX 11
#define FONA_TX 10
#define FONA_RST 9

NeoSWSerial simss(FONA_TX, FONA_RX);
Adafruit_FONA sim = Adafruit_FONA(FONA_RST);

uint8_t net_status;
boolean gprs_on = false;

char url[] = "http://102.132.252.52:7777/location";

uint16_t statuscode;
int16_t length;

AESLib aesLib;



#define INPUT_BUFFER_LIMIT (128 + 1)

unsigned char cleartext[INPUT_BUFFER_LIMIT] = {0};
unsigned char ciphertext[2 * INPUT_BUFFER_LIMIT] = {0};

const byte aes_key[] PROGMEM = {0x61, 0x72, 0x67, 0x6f, 0x73, 0x69, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21};
byte aes_iv[] = {0x61, 0x6c, 0x77, 0x61, 0x79, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21, 0x21};

void aes_init()
{
  aesLib.gen_iv(aes_iv);
  aesLib.set_paddingmode((paddingMode)0);
}

uint16_t encrypt_to_ciphertext(char *msg, uint16_t msgLen, byte iv[])
{
  int cipherlength = aesLib.encrypt((byte *)msg, msgLen, (char *)ciphertext, aes_key, sizeof(aes_key), iv);
  return cipherlength;
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

void getImei(char *imei){  
  sim.getIMEI(imei);
}


unsigned long loopcount = 0;

byte enc_iv[] = {0x61, 0x6c, 0x77, 0x61, 0x79, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21, 0x21};
byte enc_iv_to[] = {0x61, 0x6c, 0x77, 0x61, 0x79, 0x73, 0x77, 0x61, 0x74, 0x63, 0x68, 0x69, 0x6e, 0x67, 0x21, 0x21};

void sendPayload(char *payload)
{
  sprintf((char *)cleartext, "%s", payload);
  uint16_t msgLen = strlen(payload);
  memcpy(enc_iv, enc_iv_to, sizeof(enc_iv_to));
  uint16_t encLen = encrypt_to_ciphertext((char *)cleartext, msgLen, enc_iv);
  Serial.println((char *)ciphertext);
  sim.HTTP_POST_start(url, F("text/plain"), (uint8_t *)ciphertext, 75, &statuscode, (uint16_t *)&length);
  delay(100);
  sim.HTTP_POST_end();
}

void gsmStop()
{
  sim.enableGPRS(false);
  simss.end();
}
