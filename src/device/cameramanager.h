#ifndef CameraManager_h
#define CameraManager_h

#include "../lib/OV7670-ESP32/OV7670.h"

class CameraManager {
public:
    CameraManager();
    ~CameraManager();
    void init();
    void takePhoto(std::function<void(uint16_t, uint16_t, uint16_t, uint16_t*)> callback);
private:
    OV7670* cam;
    uint16_t w;
    uint16_t h;
};
#endif