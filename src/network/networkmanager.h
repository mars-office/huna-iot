#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_USE_GPRS true
// #define DUMP_AT_COMMANDS
#define TINY_GSM_RX_BUFFER 1024 

#include <TinyGsmClient.h>
#include <HardwareSerial.h>
#include <PubSubClient.h>
#include "SSLClient.h"
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
  void receiveMqttEvents();
private:
  TinyGsm* modem;
  TinyGsmClient* client;
  PubSubClient* mqtt;
  Config* config;
  SSLClient* sslClient;
};