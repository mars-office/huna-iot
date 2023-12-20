#include "networkmanager.h"
NetworkManager::NetworkManager(Config *config)
{
  this->config = config;
  this->modem = new TinyGsm(Serial2);
  this->tinyGsmClient = new TinyGsmClient(*this->modem, 0U);
  this->sslClient = new SSLClientESP32(this->tinyGsmClient);
  this->sslClient->setCACert(this->config->getCaCertificate());
  this->sslClient->setPrivateKey(this->config->getClientKey());
  this->sslClient->setCertificate(this->config->getClientCertificate());
  this->mqtt = new PubSubClient(*this->sslClient);
  this->mqtt->setServer(this->config->getMqttServer(), this->config->getMqttPort());

  this->httpClient = new HttpClient(*this->sslClient, this->config->getOtaServer(), (uint16_t)this->config->getOtaServerPort());
  this->httpClient->connectionKeepAlive();
}

NetworkManager::~NetworkManager()
{
  delete this->mqtt;
  delete this->httpClient;
  delete this->sslClient;
  delete this->tinyGsmClient;
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

char *NetworkManager::otaGetServerVersion()
{
  this->httpClient->connect(this->config->getOtaServer(), (uint16_t)this->config->getOtaServerPort());
  this->httpClient->get("/api/ota/version");
  int statusCode = this->httpClient->responseStatusCode();
  String rb = String(this->httpClient->responseBody());
  rb.trim();
  const char *bodyStr = rb.c_str();
  char *body = strdup(bodyStr);
  return statusCode == 200 ? body : nullptr;
}

void NetworkManager::otaGetLatestFirmwareBin(std::function<void (uint8_t*, size_t)> callback)
{
  this->httpClient->connect(this->config->getOtaServer(), (uint16_t)this->config->getOtaServerPort());
  this->httpClient->get("/api/ota/update");
  int statusCode = this->httpClient->responseStatusCode();
  this->httpClient->skipResponseHeaders();
  uint8_t buffer[1024];
  while (this->httpClient->available()) {
    size_t readBytesCount = this->httpClient->readBytes(buffer, 1024);
    callback(buffer, readBytesCount);
  }
}
