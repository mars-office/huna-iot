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
  Serial.println(serverVersion);
  this->fileMan->deleteFileIfExists("/ota/firmware.bin");
  File fwFile = this->fileMan->openForWrite("/ota/firmware.bin");
  this->netMan->otaGetLatestFirmwareBin([&fwFile](uint8_t* data) {
    fwFile.write(data, 1024);
  });
  fwFile.close();
  Serial.println("[OtaManager] Downloaded firmware.bin");
  Serial.println("[OtaManager] Started flashing...");
  File rFile = this->fileMan->openForRead("/ota/firmware.bin");
  if (Update.begin(rFile.size())) {
    Serial.println("[OtaManager] Update is allowed, there is space");
    Serial.println("[OtaManager] Flashing...");
    Update.writeStream(rFile);
    Serial.println("[OtaManager] Flashing done.");
    if (Update.end()) {
      Serial.println("[OtaManager] Successful update. Rebooting...");
      rFile.close();
      this->fileMan->deleteFileIfExists("/ota/firmware.bin");
      ESP.restart();
    } else {
      Serial.println("[OtaManager] Update failed.");
      rFile.close();
      this->fileMan->deleteFileIfExists("/ota/firmware.bin");
    }
  }
}
