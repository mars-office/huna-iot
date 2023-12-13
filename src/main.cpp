#include <Arduino.h>
#include "version.h"
#include "config/config.h"
#include "network/networkmanager.h"

#define LED 2

Config config;
NetworkManager netMan;

void setup()
{
  Serial.begin(115200);
  delay(10);
  Serial.print("Version:");
  Serial.println(VERSION);
  Serial.println("Setup started");
  Serial.println("Reading config...");
  config.init();
  Serial.print("ID:");
  Serial.println(config.getId());
  Serial.print("Server URL:");
  Serial.println(config.getServerUrl());
  netMan.init();
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