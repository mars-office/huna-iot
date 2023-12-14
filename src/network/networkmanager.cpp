#include "networkmanager.h"

NetworkManager::NetworkManager()
{
  this->modem = new TinyGsm(Serial2);
}

NetworkManager::~NetworkManager()
{
  delete this->modem;
}

void NetworkManager::init()
{
  Serial2.begin(115200);
  Serial.println("[NetworkManager] Initializing modem...");
  if (!this->modem->isNetworkConnected()) {
    this->modem->init();
  }
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
