#include "filemanager.h"
#include <SPIFFS.h>

FileManager::FileManager()
{
  if (!SPIFFS.begin()) {
    Serial.println("An error occurred while mounting SPIFFS");
    return;
  }
}

FileManager::~FileManager()
{
  SPIFFS.end();
}

const char* FileManager::readFile(const char *fileName)
{
  File file = SPIFFS.open(fileName, "r");
  String stringValue = file.readString();
  file.close();
  const char* dup = strdup(stringValue.c_str());
  return dup;
}

void FileManager::deleteFileIfExists(const char *fileName)
{
  if (SPIFFS.exists(fileName)) {
    SPIFFS.remove(fileName);
  }
}