#include "otamanager.h"
#include "../version.h"
#include <Update.h>

OtaManager::OtaManager(FileManager *fileMan, NetworkManager *netMan)
{
  this->fileMan = fileMan;
  this->netMan = netMan;
}

OtaManager::~OtaManager()
{
  delete this->netMan;
  delete this->fileMan;
}

void OtaManager::updateIfNecessary()
{
  Serial.println("[OtaManager] Checking for server update...");
  char* serverVersion = this->netMan->otaGetServerVersion();
  Serial.print("[OtaManager] Server version: ");
  Serial.println(serverVersion);
  if (!strcmp(serverVersion, VERSION)) {
    Serial.print("[OtaManager] No update needed, already running ");
    Serial.println(VERSION);
    return;
  }
  Serial.print("[OtaManager] Downloading firmware.bin, v");
  Serial.println(VERSION);
  this->fileMan->deleteFileIfExists("/ota/firmware.bin");
  File fwFile = this->fileMan->openForWrite("/ota/firmware.bin");
  this->netMan->otaGetLatestFirmwareBin([&fwFile](uint8_t* data) {
    fwFile.write(data, 1024);
  });
  fwFile.close();
  Serial.println("[OtaManager] Downloaded firmware.bin");
}
