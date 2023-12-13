#include "networkmanager.h"
#define MODEM_TX             27
#define MODEM_RX             26


NetworkManager::NetworkManager()
{
  this->modem = new TinyGsm(Serial1);
  
}

NetworkManager::~NetworkManager()
{
  delete this->modem;
}

void NetworkManager::init()
{
  Serial1.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);
  Serial.println("[NetworkManager] Restarting modem...");
  this->modem->restart();
  Serial.println("[NetworkManager] Modem restarted.");
}

void NetworkManager::debug()
{

}
