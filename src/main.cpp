#include <Arduino.h>
#include "version.h"
#include "network/networkmanager.h"
#include <time.h>

Config* config;
NetworkManager* netMan;
struct timeval *gsmTime;

void setup()
{
  config = new Config();
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
  netMan = new NetworkManager(config);
  netMan->init();
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
  const struct timeval tv = netMan->fetchGSMTime();
  gsmTime = new timeval(tv);
  settimeofday(gsmTime, NULL);
  netMan->ensureMqttIsConnected();
}

void loop()
{
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
  netMan->ensureMqttIsConnected();
  Serial.println("Hello!");
  netMan->receiveMqttEvents();
  delay(1000);
}