#include "config.h"
#include "../fs/filemanager.h"
#include <ArduinoJson.h>

Config::Config()
{

}

Config::~Config()
{
  delete[] this->caCertificate;
  delete[] this->clientCertificate;
  delete[] this->clientKey;
  delete[] this->id;
  delete[] this->otaServerUrl;
  delete[] this->serverUrl;
}

void Config::init()
{
  FileManager* fileManager = new FileManager();
  const char* clientCertificate = fileManager->readFile("/client.crt");
  const char* clientKey = fileManager->readFile("/client.key");
  const char* caCertificate = fileManager->readFile("/ca.crt");
  this->clientCertificate = new char[strlen(clientCertificate) + 1];
  this->clientKey = new char[strlen(clientKey) + 1];
  this->caCertificate = new char[strlen(caCertificate) + 1];
  strcpy(this->clientCertificate, clientCertificate);
  strcpy(this->clientKey, clientKey);
  strcpy(this->caCertificate, caCertificate);

  const char* config = fileManager->readFile("/config.json");
  delete fileManager;

  StaticJsonDocument<192> doc;
  DeserializationError error = deserializeJson(doc, config);
  const char* id = doc["id"];
  const char* otaServerUrl = doc["otaServerUrl"];
  const char* serverUrl = doc["serverUrl"];

  this->id = new char[strlen(id) + 1];
  this->otaServerUrl = new char[strlen(otaServerUrl) + 1];
  this->serverUrl = new char[strlen(serverUrl) + 1];

  strcpy(this->id, id);
  strcpy(this->otaServerUrl, otaServerUrl);
  strcpy(this->serverUrl, serverUrl);
}

char *Config::getServerUrl()
{
  return this->serverUrl;
}

char *Config::getOtaServerUrl()
{
  return this->otaServerUrl;
}

char *Config::getId()
{
  return this->id;
}

char *Config::getClientCertificate()
{
  return this->clientCertificate;
}

char *Config::getClientKey()
{
  return this->clientKey;
}

char *Config::getCaCertificate()
{
  return this->caCertificate;
}