#ifndef StatusMonitor_h
#define StatusMonitor_h
#include "../network/networkmanager.h"
#include "../config/config.h"

class StatusMonitor {
public:
    StatusMonitor(NetworkManager* netMan, Config* config);
    ~StatusMonitor();
    char* getStatusJson();
private:
  NetworkManager* netMan;
  Config* config;
};
#endif