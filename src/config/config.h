class Config {
public:
    Config();
    ~Config();

    void init();
    char* getServerUrl();
    char* getOtaServerUrl();
    char* getId();
    char* getClientCertificate();
    char* getClientKey();
    char* getCaCertificate();
    char* getMqttServer();
    int getMqttPort();

private:
    char* serverUrl;
    char* otaServerUrl;
    char* id;
    char* clientCertificate;
    char* clientKey;
    char* caCertificate;
    char* mqttServer;
    int mqttPort;
};