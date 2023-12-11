#include "config.h"
#include "../fs/filemanager.h"
#include <ArduinoJson.h>

Config::Config()
{

}

Config::~Config()
{
  
}

void Config::init()
{
  FileManager* fileManager = new FileManager();
  const char* clientCertificate = fileManager->readFile("/client.crt");
  const char* clientKey = fileManager->readFile("/client.key");
  const char* caCertificate = fileManager->readFile("/ca.crt");
  this->clientCertificate = clientCertificate;
  this->clientKey = clientKey;
  this->caCertificate = caCertificate;

  const char* config = fileManager->readFile("/config.json");
  delete fileManager;

  StaticJsonDocument<128> doc;
  DeserializationError error = deserializeJson(doc, config);
  const char* id = doc["id"];
  const char* otaServerUrl = doc["otaServerUrl"];
  const char* serverUrl = doc["serverUrl"];
  this->id = id;
  this->otaServerUrl = otaServerUrl;
  this->serverUrl = serverUrl;
}

const char *Config::getServerUrl()
{
  return this->serverUrl;
}

const char *Config::getOtaServerUrl()
{
  return this->otaServerUrl;
}

const char *Config::getId()
{
  return this->id;
}

const char *Config::getClientCertificate()
{
  return this->clientCertificate;
}

const char *Config::getClientKey()
{
  return this->clientKey;
}

const char *Config::getCaCertificate()
{
  return this->caCertificate;
}
