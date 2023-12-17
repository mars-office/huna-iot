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
  this->pubsubTinyGsmClient = new TinyGsmClient(*this->modem, 0U);
  this->pubsubSslClient = new SSLClientESP32(this->pubsubTinyGsmClient);
  this->pubsubSslClient->setCACert(this->config->getCaCertificate());
  this->pubsubSslClient->setPrivateKey(this->config->getClientKey());
  this->pubsubSslClient->setCertificate(this->config->getClientCertificate());
  this->mqtt = new PubSubClient(*this->pubsubSslClient);
  this->mqtt->setServer(this->config->getMqttServer(), this->config->getMqttPort());

  this->httpSslClient = new SSLClientESP32(this->pubsubTinyGsmClient);
  this->httpSslClient->setCACert(this->config->getLetsencryptCaCertificate());
  this->httpSslClient->setPrivateKey(this->config->getClientKey());
  this->httpSslClient->setCertificate(this->config->getClientCertificate());
  this->httpClient = new HttpClient(*this->httpSslClient, String(this->config->getServer()), (uint16_t)this->config->getServerPort());
}

NetworkManager::~NetworkManager()
{
  delete this->mqtt;
  delete this->httpClient;
  delete this->httpSslClient;
  delete this->pubsubSslClient;
  delete this->pubsubTinyGsmClient;
  delete this->modem;
}

void NetworkManager::init()
{
  Serial2.begin(115200);
  Serial.println("[NetworkManager] Initializing modem...");
  this->modem->init();
  delay(1000);
  Serial.println("[NetworkManager] Modem initialized.");
}

void NetworkManager::ensureRegistrationOnNetwork()
{
  while (!this->modem->waitForNetwork())
  {
    Serial.println("[NetworkManager] No network, waiting 60s...");
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

const struct timeval NetworkManager::fetchGSMTime()
{
  int hh, mm, ss, yy, mon, day, tz;
  struct tm when = {0};
  time_t epoch = 0;
  String currentDt = this->modem->getGSMDateTime(DATE_FULL);
  sscanf(currentDt.c_str(), "%d/%d/%d,%d:%d:%d+%d", &yy, &mon, &day, &hh, &mm, &ss, &tz);
  when.tm_hour = hh;
  when.tm_min = mm;
  when.tm_sec = ss;
  when.tm_year = 2000 + yy - 1900;

  when.tm_mon = mon - 1;
  when.tm_mday = day;
  when.tm_isdst = -1;
  epoch = mktime(&when);
  struct timeval tv;
  tv.tv_sec = epoch + 1;
  tv.tv_usec = 0;
  return tv;
}

void NetworkManager::receiveMqttEvents()
{
  this->mqtt->loop();
}

void NetworkManager::setMqttCallback(std::function<void(char *, uint8_t *, unsigned int)> cb)
{
  this->mqtt->setCallback(cb);
}

bool NetworkManager::mqttSubscribe(const char *topic, uint8_t qos)
{
  return this->mqtt->subscribe(topic, qos);
}

bool NetworkManager::mqttUnsubscribe(const char *topic)
{
  return this->mqtt->unsubscribe(topic);
}

char *NetworkManager::httpGetString(bool useOtaServer, const char *url)
{
  char *result = nullptr;
  if (this->httpClient->connect(useOtaServer ? this->config->getOtaServer() : this->config->getServer(),
                      useOtaServer ? this->config->getOtaServerPort() : this->config->getServerPort()))
  {
    if (this->httpClient->get(url) == 0 && this->httpClient->responseStatusCode() == 200)
    {
      const char *response = this->httpClient->responseBody().c_str();
      result = new char[strlen(response) + 1];
      strcpy(result, response);
    }
    this->httpClient->stop();
  }
  return result;
}