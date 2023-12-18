#include "../network/networkmanager.h"

class OtaManager {
public:
    OtaManager(FileManager* fileMan, NetworkManager* netMan);
    ~OtaManager();
    void updateIfNecessary();

private:
    NetworkManager* netMan;
    FileManager* fileMan;
};