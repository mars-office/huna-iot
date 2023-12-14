#define TINY_GSM_MODEM_SIM800

#include <TinyGsmClient.h>
#include <HardwareSerial.h>

class NetworkManager
{
public:
  NetworkManager();
  ~NetworkManager();
  void init();
  void ensureRegistrationOnNetwork();
  void ensureGprsIsConnected();

private:
  TinyGsm* modem;
};