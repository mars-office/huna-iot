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
  delay(3000);
  Serial.println("[NetworkManager] Restarting modem...");
  this->modem->init();
  Serial.println("[NetworkManager] Modem restarted.");
  
}

void NetworkManager::debug()
{
  Serial.print("[NetworkManager] Modem debug:");
  Serial.println(this->modem->getModemInfo());
  Serial.println(this->modem->getOperator());
  Serial.println(this->modem->getRegistrationStatus());
  Serial.println(this->modem->getSimStatus());
  Serial.println(this->modem->getSignalQuality());
}
