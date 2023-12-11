#include <Arduino.h>
#include "version.h"
#include "config/config.h"

#define LED 2

Config config;

void setup()
{
  Serial.begin(115200);

  Serial.print("Version:");
  Serial.println(VERSION);

  Serial.println("Setup started");
  Serial.println("Reading config...");
  config.init();
  
  Serial.print("ID:");
  Serial.println(config.getId());
  Serial.print("Server URL:");
  Serial.println(config.getServerUrl());

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