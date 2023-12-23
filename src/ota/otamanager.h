#ifndef OtaManager_h
#define OtaManager_h
#include "../network/networkmanager.h"
#include "../fs/filemanager.h"

class OtaManager {
public:
    OtaManager(FileManager* fileMan, NetworkManager* netMan);
    ~OtaManager();
    void updateIfNecessary(bool forceUpdate);

private:
    NetworkManager* netMan;
    FileManager* fileMan;
};
#endif