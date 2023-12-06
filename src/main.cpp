#include <Arduino.h>
#include "version.h"
#define LED 2

void setup()
{
  Serial.begin(9600);
  Serial.println(VERSION);
  Serial.println("Setup started");
  pinMode(LED, OUTPUT);
}

void loop()
{
  delay(1000);
  digitalWrite(LED, HIGH);
  delay(1000);
  digitalWrite(LED, LOW);
  Serial.println("Hello!");
}