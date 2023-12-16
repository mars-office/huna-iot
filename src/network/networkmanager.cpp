#include "networkmanager.h"
#ifdef DUMP_AT_COMMANDS
#include <StreamDebugger.h>
#endif
NetworkManager::NetworkManager(Config *config)
{
  this->config = config;
#ifdef DUMP_AT_COMMANDS
  StreamDebugger *debugger = new StreamDebugger(Serial2, Serial);
  this->modem = new TinyGsm(*debugger);
#else
  this->modem = new TinyGsm(Serial2);
#endif

  this->client = new TinyGsmClient(*this->modem);
  //this->sslClient = new SSLClient(this->client);
  //this->sslClient->setCACert(this->config->getCaCertificate());
  //this->sslClient->setPrivateKey(this->config->getClientKey());
  //this->sslClient->setCertificate(this->config->getClientCertificate());
  this->mqtt = new PubSubClient(*this->client);
  Serial.println("[NetworkManager] Setting up MQTT...");
  this->mqtt->setServer(this->config->getMqttServer(), this->config->getMqttPort());
}

NetworkManager::~NetworkManager()
{
  delete this->mqtt;
  delete this->sslClient;
  delete this->client;
  delete this->modem;
}

void NetworkManager::init()
{
  Serial2.begin(115200);
  Serial.println("[NetworkManager] Initializing modem...");
  if (!this->modem->isNetworkConnected())
  {
    this->modem->init();
  }
  delay(1000);
  Serial.println("[NetworkManager] Modem initialized.");
}

void NetworkManager::ensureRegistrationOnNetwork()
{
  while (!this->modem->waitForNetwork(10000))
  {
    Serial.println("[NetworkManager] No network, waiting 10s...");
  }
}

void NetworkManager::ensureGprsIsConnected()
{
  while (!this->modem->isGprsConnected())
  {
    Serial.println("[NetworkManager] GPRS not connected");
    Serial.println("[NetworkManager] GPRS connecting...");
    if (!this->modem->gprsConnect("net"))
    {
      Serial.println("[NetworkManager] GPRS connection failed.");
    }
    else
    {
      Serial.println("[NetworkManager] GPRS connected");
      break;
    }
    Serial.println("[NetworkManager] Waiting 5s");
    delay(5000);
  }
}

void NetworkManager::ensureMqttIsConnected()
{
  while (!this->mqtt->connected())
  {
    Serial.println("[NetworkManager] MQTT not connected.");
    if (!this->mqtt->connect(this->config->getId()))
    {
      Serial.println("[NetworkManager] MQTT connection failed. Waiting 5s...");
      delay(5000);
    }
    else
    {
      Serial.println("[NetworkManager] MQTT connected.");
    }
  }
}

void NetworkManager::mqttCallback(char *topic, uint8_t *payload, unsigned int length)
{

  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}
