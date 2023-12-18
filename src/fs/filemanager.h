#include <SPIFFS.h>

class FileManager {
public:
    FileManager();
    ~FileManager();

    const char* readFile(const char* fileName);
    void deleteFileIfExists(const char* fileName);
    File openForWrite(const char* fileName);
    File openForRead(const char* fileName);
};