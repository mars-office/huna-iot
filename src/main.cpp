#include <Arduino.h>
#include "version.h"
#include "config/config.h"

#define LED 2

Config config;

void setup()
{
  Serial.begin(9600);

  Serial.println(VERSION);
  Serial.println("Setup started");

  config.init();

  pinMode(LED, OUTPUT);
}

void loop()
{
  delay(1000);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  Serial.println("Hello!");
}