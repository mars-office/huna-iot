#include <Arduino.h>
#include "version.h"
#include "config/config.h"
#include "network/networkmanager.h"

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
  netMan.ensureRegistrationOnNetwork();
  netMan.ensureGprsIsConnected();
}

void loop()
{
  netMan.ensureRegistrationOnNetwork();
  netMan.ensureGprsIsConnected();
  Serial.println("Hello!");
  delay(1000);
}