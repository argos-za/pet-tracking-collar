#include "AESLib.h"

#define BAUD 9600

#define FONA_RX 11
#define FONA_TX 10
#define FONA_RST 9

NeoSWSerial simss(FONA_TX, FONA_RX);
Adafruit_FONA sim = Adafruit_FONA(FONA_RST);

uint8_t net_status;
boolean gprs_on = false;

char url[] = "api.thingspeak.com/update?api_key=OBMUL3QGC46S3T0L";

uint16_t statuscode;
int16_t length;

AESLib aesLib;

#define INPUT_BUFFER_LIMIT (128 + 1)

unsigned char cleartext[INPUT_BUFFER_LIMIT] = {0};
unsigned char ciphertext[2*INPUT_BUFFER_LIMIT] = {0};

byte aes_key[] = { 0x61,0x72,0x67,0x6f,0x73,0x69,0x73,0x77,0x61,0x74,0x63,0x68,0x69,0x6e,0x67,0x21 };

byte aes_iv[N_BLOCK] = { 0x61,0x6c,0x77,0x61,0x79,0x73,0x77,0x61,0x74,0x63,0x68,0x69,0x6e,0x67,0x21,0x21 };

// Generate IV (once)
void aes_init() {
  aesLib.gen_iv(aes_iv);
  aesLib.set_paddingmode((paddingMode)0);
}

uint16_t encrypt_to_ciphertext(char * msg, uint16_t msgLen, byte iv[]) {
  int cipherlength = aesLib.encrypt((byte*)msg, msgLen, (char*)ciphertext, aes_key, sizeof(aes_key), iv);
  return cipherlength;
}

/* non-blocking wait function */
void wait(unsigned long milliseconds) {
  unsigned long timeout = millis() + milliseconds;
  while (millis() < timeout) {
    yield();
  }
}

void gsmStart()
{
  simss.begin(BAUD);

  if (! sim.begin(simss)) {
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

unsigned long loopcount = 0;

// Working IV buffer: Will be updated after encryption to follow up on next block.
// But we don't want/need that in this test, so we'll copy this over with enc_iv_to/enc_iv_from
// in each loop to keep the test at IV iteration 1. We could go further, but we'll get back to that later when needed.

// General initialization vector (same as in node-js example) (you must use your own IV's in production for full security!!!)
byte enc_iv[N_BLOCK] =      { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
byte enc_iv_to[N_BLOCK]   = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };
byte enc_iv_from[N_BLOCK] = { 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA };

void sendPayload(char *payload)
{  
  if (!gprs_on)
  {
    gsmStart();
  }
  else
  {
    // must not exceed INPUT_BUFFER_LIMIT bytes; may contain a newline
    sprintf((char*)cleartext, "%s", payload);
  
    // Encrypt
    // iv_block gets written to, provide own fresh copy... so each iteration of encryption will be the same.
    uint16_t msgLen = sizeof(payload);
    memcpy(enc_iv, enc_iv_to, sizeof(enc_iv_to));
    uint16_t encLen = encrypt_to_ciphertext((char*)cleartext, msgLen, enc_iv);
    sim.HTTP_POST_start(url, F("text/plain"), (uint8_t *)ciphertext, encLen, &statuscode, (uint16_t *)&length);
    while (length > 0)
    {
      while (sim.available())
      {
        char c = sim.read();
      }
    }
  }
}

void gsmStop()
{  
  sim.enableGPRS(false);
  simss.end();
}
