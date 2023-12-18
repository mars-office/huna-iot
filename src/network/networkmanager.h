#define TINY_GSM_MODEM_SIM800
// #define DUMP_AT_COMMANDS
// #define TINY_GSM_RX_BUFFER 1024

#include <TinyGsmClient.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include "SSLClientESP32.h"
#include "../config/config.h"

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
  void httpGetString(const char* host, uint16_t port, const char* resource);
private:
  TinyGsm* modem;
  TinyGsmClient* tinyGsmClient;
  PubSubClient* mqtt;
  Config* config;
  SSLClientESP32* sslClient;
};