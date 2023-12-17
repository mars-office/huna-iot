#include <Arduino.h>
#include "version.h"
#include "network/networkmanager.h"
#include <time.h>

Config* config;
NetworkManager* netMan;

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
  netMan->ntpSync();
  netMan->ensureMqttIsConnected();
}

void setClock() {
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
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