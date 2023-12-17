#include <Arduino.h>
#include "version.h"
#include "network/networkmanager.h"
#include <time.h>

Config *config;
NetworkManager *netMan;
struct timeval *gsmTime;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

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
  Serial.println(config->getServer());
  netMan = new NetworkManager(config);
  netMan->init();
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
  const struct timeval tv = netMan->fetchGSMTime();
  gsmTime = new timeval(tv);
  settimeofday(gsmTime, NULL);
  netMan->setMqttCallback(mqttCallback);
  //netMan->ensureMqttIsConnected();
  delay(500);
  // if (!netMan->mqttSubscribe("gigel", 0)) {
  //   Serial.println("Could not subscribe to topic");
  // }
  Serial.println(netMan->httpGetString(true, "/api/ota/version"));
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
