#include <Arduino.h>
#include "version.h"
#include "ota/otamanager.h"
#include <time.h>
#include "device/internalledmanager.h"

FileManager* fileMan;
Config *config;
NetworkManager *netMan;
struct timeval *gsmTime;
OtaManager* ota;
InternalLedManager* internalLed;

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
  internalLed = new InternalLedManager();
  internalLed->init();
  internalLed->on();
  fileMan = new FileManager();
  config = new Config(fileMan);
  Serial.begin(115200);
  delay(10);
  Serial.print("Version:");
  Serial.println(VERSION);
  Serial.println("Setup started");
  Serial.println("Reading config...");
  config->init();
  Serial.print("ID:");
  Serial.println(config->getId());
  Serial.print("Detection Server URL:");
  Serial.println(config->getDetectionServer());
  netMan = new NetworkManager(config);
  ota = new OtaManager(fileMan, netMan);
  netMan->init();
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
  
  const struct timeval tv = netMan->fetchGSMTime();
  gsmTime = new timeval(tv);
  settimeofday(gsmTime, NULL);
  
  ota->updateIfNecessary();

  netMan->setMqttCallback(mqttCallback);
  netMan->ensureMqttIsConnected();
  delay(500);
  if (!netMan->mqttSubscribe("gigel", 0))
  {
    Serial.println("Could not subscribe to topic");
  }
  internalLed->off();
  delay(500);
}

void loop()
{
  internalLed->on();
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
  netMan->ensureMqttIsConnected();
  Serial.println("Hello!");
  netMan->receiveMqttEvents();
  internalLed->off();
  delay(1000);
}
