class Config {
public:
    Config();
    ~Config();

    void init();
    const char* getServerUrl();
    const char* getOtaServerUrl();
    const char* getId();
    const char* getClientCertificate();
    const char* getClientKey();
    const char* getCaCertificate();

private:
    const char* serverUrl;
    const char* otaServerUrl;
    const char* id;
    const char* clientCertificate;
    const char* clientKey;
    const char* caCertificate;
};