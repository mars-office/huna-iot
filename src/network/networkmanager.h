#define TINY_GSM_MODEM_SIM800
#define TINY_GSM_RX_BUFFER 1030

#include <TinyGsmClient.h>
#include <HardwareSerial.h>

class NetworkManager
{
public:
  NetworkManager();
  ~NetworkManager();
  void init();
  void debug();

private:
  TinyGsm* modem;
};