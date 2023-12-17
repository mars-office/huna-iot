#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_USE_GPRS true
// #define DUMP_AT_COMMANDS
#define TINY_GSM_RX_BUFFER 1024 

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
  char* httpGetString(bool useOtaServer, const char* url);
private:
  TinyGsm* modem;
  TinyGsmClient* pubsubTinyGsmClient;
  TinyGsmClient* httpTinyGsmClient;
  TinyGsmClient* httpOtaTinyGsmClient;
  PubSubClient* mqtt;
  Config* config;
  SSLClientESP32* pubsubSslClient;
  SSLClientESP32* httpSslClient;
  SSLClientESP32* httpOtaSslClient;
  HttpClient* otaHttpClient;
  HttpClient* httpClient;
  HttpClient* getHttpClient(bool useOtaServer);
};