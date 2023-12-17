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
    char* getLetsencryptCaCertificate();
private:
    char* server;
    char* otaServer;
    int otaServerPort;
    int serverPort;
    char* id;
    char* clientCertificate;
    char* clientKey;
    char* caCertificate;
    char* letsencryptCaCertificate;
    char* mqttServer;
    int mqttPort;
};