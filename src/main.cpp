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
#include "device/cameramanager.h"

FileManager* fileMan;
Config *config;
NetworkManager *netMan;
struct timeval *gsmTime;
OtaManager* ota;
StatusMonitor* statusMonitor;
InternalLedManager* internalLed;
CameraManager* cameraManager;

unsigned long lastStatusMillis = 0;
unsigned long lastOtaCheckMillis = 0;
unsigned long lastPhotoMillis = 0;
const char* statusTopic;


void handleCommand(String payload) {
  payload.toLowerCase();
  if (payload == "reboot") {
    Serial.println("[Command] Rebooting...");
    ESP.restart();
    return;
  }
  if (payload == "otacheck") {
    Serial.println("[Command] OTA Check...");
    ota->updateIfNecessary(false);
    return;
  }
  if (payload == "forcedota") {
    Serial.println("[Command] Forced OTA Update...");
    ota->updateIfNecessary(true);
    return;
  }
  if (payload == "resetmodem") {
    Serial.println("[Command] Reset modem...");
    netMan->hardResetModem();
    return;
  }

  Serial.println("[Command] Unknown command");
}


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
  Serial.println("Sending initial status event...");
  netMan->publishMqttMessage(statusTopic, statusMonitor->getStatusJson());
  Serial.println("Initial status event sent.");
  ota->updateIfNecessary(false);

  cameraManager = new CameraManager();
  cameraManager->init();

  internalLed->off();
  Serial.println("Setup done");
}

void loop()
{
  internalLed->on();
  netMan->ensureRegistrationOnNetwork();
  netMan->ensureGprsIsConnected();
  netMan->ensureMqttIsConnected();
  netMan->receiveMqttEvents();
  
  unsigned long currentMillis = millis();

  if (currentMillis - lastStatusMillis >= 25000) {
    lastStatusMillis = currentMillis;
    Serial.println("Publishing status message...");
    if (netMan->publishMqttMessage(statusTopic, statusMonitor->getStatusJson())) {
      Serial.println("Published status message.");
    } else {
      Serial.println("Publishing status message failed.");
    }
  }

  if (currentMillis - lastOtaCheckMillis >= 3600000L) {
    Serial.println("Checking for OTA updates...");
    lastOtaCheckMillis = currentMillis;
    ota->updateIfNecessary(false);
  }

  if (currentMillis - lastPhotoMillis >= 30000L) {
    Serial.println("Taking photo...");
    lastPhotoMillis = currentMillis;
    cameraManager->takePhoto();
  }

  internalLed->off();
  delay(1000);
}
