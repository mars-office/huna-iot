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
  char *serverVersion = this->netMan->otaGetServerVersion();
  Serial.print("[OtaManager] Server version: ");
  Serial.println(serverVersion);
  if (serverVersion == nullptr)
  {
    Serial.println("[OtaManager] Cannot get latest version from server");
    return;
  }
  if (!strcmp(serverVersion, VERSION))
  {
    Serial.print("[OtaManager] No update needed, already running ");
    Serial.println(VERSION);
    return;
  }
  Serial.print("[OtaManager] Downloading firmware.bin, v");
  Serial.println(serverVersion);
  this->fileMan->deleteFileIfExists("/ota/firmware.bin");
  File fwFile = this->fileMan->openForWrite("/ota/firmware.bin");
  int downloaded = 0;
  this->netMan->otaGetLatestFirmwareBin([&fwFile, &downloaded](uint8_t *data)
                                        { 
                                          int written = (int)(fwFile.write(data, 1024));
                                          downloaded += written;
                                          Serial.print("[OtaManager] Downloaded ");
                                          Serial.print(downloaded);
                                          Serial.print(" bits"); Serial.println(); });
  fwFile.close();
  Serial.println("[OtaManager] Downloaded firmware.bin");
  Serial.println("[OtaManager] Started flashing...");
  File rFile = this->fileMan->openForRead("/ota/firmware.bin");
  int fileSize = (int)rFile.size();
  if (!Update.begin(fileSize))
  {
    Serial.println("[OtaManager] Update is not allowed.");
    Serial.println(Update.errorString());
    Serial.println(Update.getError());
    rFile.close();
    return;
  }
  Serial.println("[OtaManager] Update is allowed, there is space");
  Serial.println("[OtaManager] Uploading...");
  uint8_t buff[1024];
  int flashed = 0;
  while (rFile.available())
  {
    rFile.read(buff, 1024);
    Serial.println((char *)buff);
    int written = (int)(Update.write(buff, 1024));
    flashed += written;
    Serial.print("[OtaManager] Flashed ");
    Serial.print(flashed);
    Serial.print("/");
    Serial.print(fileSize);
    Serial.println();
  }
  Serial.println("[OtaManager] Upload done.");
  if (Update.end())
  {
    Serial.println("[OtaManager] Closing file handle...");
    rFile.close();
    Serial.println("[OtaManager] Deleting temp file...");
    this->fileMan->deleteFileIfExists("/ota/firmware.bin");
    delay(4000);
    Serial.println("[OtaManager] Successful update. Rebooting...");
    ESP.restart();
  }
  else
  {
    Serial.println("[OtaManager] Update failed.");
    Serial.println(Update.getError());
    Serial.println(Update.errorString());
    Serial.println("[OtaManager] Closing file handle...");
    rFile.close();
    Serial.println("[OtaManager] Deleting temp file...");
    this->fileMan->deleteFileIfExists("/ota/firmware.bin");
  }
}
