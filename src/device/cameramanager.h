#ifndef CameraManager_h
#define CameraManager_h

#include "../lib/OV7670-ESP32/OV7670.h"

class CameraManager {
public:
    CameraManager();
    ~CameraManager();
    void init();
    void takePhoto();
private:
    OV7670* cam;
    uint16_t *buf;
    uint16_t w;
    uint16_t h;
};
#endif