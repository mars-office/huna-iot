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
  this->mqtt->setBufferSize(1024);

  this->tinyGsmClient2 = new TinyGsmClient(*this->modem, 1U);
  this->sslClient2 = new SSLClientESP32(this->tinyGsmClient2);
  this->sslClient2->setCACert(this->config->getCaCertificate());
  this->sslClient2->setPrivateKey(this->config->getClientKey());
  this->sslClient2->setCertificate(this->config->getClientCertificate());

}

NetworkManager::~NetworkManager()
{
  delete this->mqtt;
  delete this->sslClient2;
  delete this->sslClient;
  delete this->tinyGsmClient;
  delete this->tinyGsmClient2;
  delete this->modem;
}

void NetworkManager::init()
{
  pinMode(GPIO_NUM_4, OUTPUT);
  digitalWrite(GPIO_NUM_4, LOW);
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
  HttpClient* httpClient = new HttpClient(*this->sslClient2, this->config->getOtaServer(), (uint16_t)this->config->getOtaServerPort());
  httpClient->connectionKeepAlive();
  httpClient->connect(this->config->getOtaServer(), (uint16_t)this->config->getOtaServerPort());
  httpClient->get("/api/ota/version");
  int statusCode = httpClient->responseStatusCode();
  String rb = String(httpClient->responseBody());
  rb.trim();
  const char *bodyStr = rb.c_str();
  char *body = strdup(bodyStr);
  httpClient->stop();
  delete httpClient;
  return statusCode == 200 ? body : nullptr;
}

void NetworkManager::otaGetLatestFirmwareBin(std::function<void (uint8_t*, size_t)> callback)
{
  HttpClient* httpClient = new HttpClient(*this->sslClient2, this->config->getOtaServer(), (uint16_t)this->config->getOtaServerPort());
  httpClient->connectionKeepAlive();
  httpClient->connect(this->config->getOtaServer(), (uint16_t)this->config->getOtaServerPort());
  httpClient->get("/api/ota/update");
  int statusCode = httpClient->responseStatusCode();
  httpClient->skipResponseHeaders();
  uint8_t buffer[1024];
  while (httpClient->available()) {
    size_t readBytesCount = httpClient->readBytes(buffer, 1024);
    callback(buffer, readBytesCount);
  }
  httpClient->stop();
  delete httpClient;
}

bool NetworkManager::publishMqttMessage(const char *topic, const char *data)
{
  return this->mqtt->publish(topic, data);
}

int NetworkManager::getGsmSignal()
{
  return (int)this->modem->getSignalQuality();
}

char *NetworkManager::getSimSerial()
{
  String ccid = this->modem->getSimCCID();
  const char* ccidChar = ccid.c_str();
  return strdup(ccidChar);
}

char *NetworkManager::getOperator()
{
  String opr = this->modem->getOperator();
  const char* oprChar = opr.c_str();
  return strdup(oprChar);
}

int NetworkManager::getModemVoltage()
{
  return (int)this->modem->getBattVoltage();
}

char *NetworkManager::getModemImei()
{
  String imei = this->modem->getIMEI();
  const char* imeiChar = imei.c_str();
  return strdup(imeiChar);
}

void NetworkManager::hardResetModem()
{
  digitalWrite(GPIO_NUM_4, HIGH);
  delay(1005);
  digitalWrite(GPIO_NUM_4, LOW);
}
