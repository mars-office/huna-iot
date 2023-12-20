#include <Arduino.h>
#include "version.h"
#include "ota/otamanager.h"
#include "config/config.h"
#include "network/networkmanager.h"
#include "fs/filemanager.h"
#include <time.h>
#include "version.h"
#include "device/internalledmanager.h"
#include "device/statusmonitor.h"

FileManager* fileMan;
Config *config;
NetworkManager *netMan;
struct timeval *gsmTime;
OtaManager* ota;
StatusMonitor* statusMonitor;
InternalLedManager* internalLed;

unsigned long lastStatusMillis = 0;
const char* statusTopic;

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
  String topic = "devices/";
  topic.concat(config->getId());
  statusTopic = strdup(topic.c_str());
  Serial.print("ID:");
  Serial.println(config->getId());
  Serial.print("Detection Server URL:");
  Serial.println(config->getDetectionServer());
  netMan = new NetworkManager(config);
  statusMonitor = new StatusMonitor(netMan, config);
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
  
  unsigned long currentMillis = millis();
  if (currentMillis - lastStatusMillis >= 25000) {
    lastStatusMillis = currentMillis;
    Serial.println("Publishing status message...");
    Serial.println(statusTopic);
    Serial.println(statusMonitor->getStatusJson());
    if (netMan->publishMqttMessage(statusTopic, statusMonitor->getStatusJson())) {
      Serial.println("Published status message.");
    } else {
      Serial.println("Published status message failed.");
    }
  }

  internalLed->off();
  delay(1000);
}
