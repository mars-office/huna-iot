#include "statusmonitor.h"
#include "../version.h"


StatusMonitor::StatusMonitor(NetworkManager* netMan, Config* config)
{
  this->netMan = netMan;
  this->config = config;
}

StatusMonitor::~StatusMonitor()
{
  delete this->netMan;
  delete this->config;
}

char *StatusMonitor::getStatusJson()
{
  struct timeval tv;
  gettimeofday(&tv, nullptr);
  int unixTs = (int)tv.tv_sec;
  String json = "{";
  json.concat("\"signal\":");
  json.concat(this->netMan->getGsmSignal());
  json.concat(",");
  json.concat("\"version\":");
  json.concat("\"");
  json.concat(VERSION);
  json.concat("\"");
  json.concat(",");
  json.concat("\"freeheap\":");
  json.concat(ESP.getFreeHeap());
  json.concat(",");
  json.concat("\"cpufreq\":");
  json.concat(ESP.getCpuFreqMHz());
  json.concat(",");
  json.concat("\"id\":");
  json.concat("\"");
  json.concat(this->config->getId());
  json.concat("\"");
  json.concat(",");
  json.concat("\"uptimemillis\":");
  json.concat(millis());
  json.concat(",");
  json.concat("\"localtime\":");
  json.concat(unixTs);
  json.concat(",");
  json.concat("\"modemimei\":");
  json.concat("\"");
  json.concat(this->netMan->getModemImei());
  json.concat("\"");
  json.concat(",");
  json.concat("\"operator\":");
  json.concat("\"");
  json.concat(this->netMan->getOperator());
  json.concat("\"");
  json.concat(",");
  json.concat("\"simserial\":");
  json.concat("\"");
  json.concat(this->netMan->getSimSerial());
  json.concat("\"");
  json.concat(",");
  json.concat("\"modemvlt\":");
  json.concat(this->netMan->getModemVoltage());
  json.concat("}");
  const char* result = json.c_str();
  return strdup(result);
}
