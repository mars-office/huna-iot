#include <Arduino.h>
#include "version.h"
#include "network/networkmanager.h"
#include <time.h>

Config* config;
NetworkManager* netMan;

void setup()
{
  config = new Config();
  netMan = new NetworkManager(config);
  Serial.begin(115200);
  delay(10);
  Serial.print("Version:");
  Serial.println(VERSION);
  Serial.println("Setup started");
  Serial.println("Reading config...");
  config->init();
  Serial.print("ID:");
  Serial.println(config->getId());
  Serial.print("Server URL:");
  Serial.println(config->getServerUrl());
  netMan->init();
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
}

void loop()
{
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
  netMan->ensureMqttIsConnected();
  Serial.println("Hello!");
  
  delay(60000);
}