#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include "SSLClientESP32.h"
#include "../config/config.h"
#include <ArduinoHttpClient.h>

class NetworkManager
{
public:
  NetworkManager(Config* config);
  ~NetworkManager();
  void init();
  void ensureRegistrationOnNetwork();
  void ensureGprsIsConnected();
  void ensureMqttIsConnected();
  const struct timeval fetchGSMTime();
  void receiveMqttEvents();
  void setMqttCallback(std::function<void (char*,uint8_t*,unsigned int)> cb);
  bool mqttSubscribe(const char *topic, uint8_t qos);
  bool mqttUnsubscribe(const char* topic);
  char* otaGetServerVersion();
private:
  TinyGsm* modem;
  TinyGsmClient* tinyGsmClient;
  PubSubClient* mqtt;
  Config* config;
  SSLClientESP32* sslClient;
  HttpClient* httpClient;
};