class Config {
public:
    Config();
    ~Config();

    void init();
    char* getServer();
    char* getOtaServer();
    char* getId();
    char* getClientCertificate();
    char* getClientKey();
    char* getCaCertificate();
    char* getMqttServer();
    int getMqttPort();
    int getOtaServerPort();
    int getServerPort();
private:
    char* server;
    char* otaServer;
    int otaServerPort;
    int serverPort;
    char* id;
    char* clientCertificate;
    char* clientKey;
    char* caCertificate;
    char* mqttServer;
    int mqttPort;
};