char *behind(char *buff)
{
  return buff + strlen(buff);
}

void flushSerial() {
  while (Serial.available())
    Serial.read();
}
