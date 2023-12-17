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
  delete[] this->otaServer;
  delete[] this->detectionServer;
  delete[] this->mqttServer;
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

  StaticJsonDocument<384> doc;
  deserializeJson(doc, config);
  const char* id = doc["id"];
  const char* otaServer = doc["otaServer"];
  const char* detectionServer = doc["detectionServer"];
  const char* mqttServer = doc["mqttServer"];

  this->id = new char[strlen(id) + 1];
  this->otaServer = new char[strlen(otaServer) + 1];
  this->detectionServer = new char[strlen(detectionServer) + 1];
  this->mqttServer = new char[strlen(mqttServer) + 1];

  strcpy(this->id, id);
  strcpy(this->otaServer, otaServer);
  strcpy(this->detectionServer, detectionServer);
  strcpy(this->mqttServer, mqttServer);
  this->mqttPort = doc["mqttPort"].as<int>();
  this->otaServerPort = doc["otaServerPort"].as<int>();
  this->detectionServerPort = doc["detectionServerPort"].as<int>();
}

char *Config::getDetectionServer()
{
  return this->detectionServer;
}

char *Config::getOtaServer()
{
  return this->otaServer;
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

char *Config::getMqttServer()
{
  return this->mqttServer;
}

int Config::getMqttPort()
{
  return this->mqttPort;
}

int Config::getOtaServerPort()
{
  return this->otaServerPort;
}

int Config::getDetectionServerPort()
{
  return this->detectionServerPort;
}
