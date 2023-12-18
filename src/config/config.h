#include "../fs/filemanager.h"

class Config {
public:
    Config(FileManager* fm);
    ~Config();

    void init();
    char* getDetectionServer();
    char* getOtaServer();
    char* getId();
    char* getClientCertificate();
    char* getClientKey();
    char* getCaCertificate();
    char* getMqttServer();
    int getMqttPort();
    int getOtaServerPort();
    int getDetectionServerPort();
private:
    FileManager* fileMan;
    char* detectionServer;
    char* otaServer;
    int otaServerPort;
    int detectionServerPort;
    char* id;
    char* clientCertificate;
    char* clientKey;
    char* caCertificate;
    char* mqttServer;
    int mqttPort;
};