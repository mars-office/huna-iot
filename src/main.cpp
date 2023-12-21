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
unsigned long lastOtaCheckMillis = 0;
const char* statusTopic;
const char* commandsTopic;

void mqttCallback(char *topic, byte *payload, unsigned int length)
{
  String topicStr = String(topic);
  String payloadStr = String(payload, length);
  Serial.print("Message arrived [");
  Serial.print(topicStr);
  Serial.print("] ");
  Serial.print(payloadStr);
  Serial.println();
  if (topicStr.startsWith("commands/")) {
    handleCommand(payloadStr);
    return;
  }
}

void handleCommand(String payload) {
  payload.toLowerCase();
  if (payload == "reboot") {
    Serial.println("[Command] Rebooting...");
    ESP.restart();
    return;
  }
  if (payload == "otacheck") {
    Serial.println("[Command] OTA Check...");
    ota->updateIfNecessary();
    return;
  }
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
  String ststopic = "status/";
  ststopic.concat(config->getId());
  statusTopic = strdup(ststopic.c_str());
  String cmdtopic = "commands/";
  cmdtopic.concat(config->getId());
  commandsTopic = strdup(cmdtopic.c_str());
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

  netMan->setMqttCallback(mqttCallback);
  netMan->ensureMqttIsConnected();
  delay(500);
  if (!netMan->mqttSubscribe(commandsTopic, 0))
  {
    Serial.println("Could not subscribe to commands topic");
  }
  Serial.println("Sending initial status event...");
  netMan->publishMqttMessage(statusTopic, statusMonitor->getStatusJson());
  Serial.println("Initial status event sent.");
  delay(100);
  ota->updateIfNecessary();

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
      Serial.println("Publishing status message failed.");
    }
  }

  if (currentMillis - lastOtaCheckMillis >= 3600000L) {
    Serial.println("Checking for OTA updates...");
    lastOtaCheckMillis = currentMillis;
    ota->updateIfNecessary();
  }

  internalLed->off();
  delay(1000);
}
