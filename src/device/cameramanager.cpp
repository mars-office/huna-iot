#define MODE QVGA // 320 X 240
// #define MODE      QQVGA  // 160 x 120
// #define MODE      QCIF  // 176 x 144 (crop)
// #define MODE      QQCIF  //  88 x 72 (crop)

#define COLOR RGB565
// #define COLOR     YUV422

#include <Wire.h>
#include <SPI.h>
#include "cameramanager.h"
#include <Arduino.h>

// OV7670 pins
// SSCB_SDA(SIOD) -> 21(ESP32)
// SSCB_SCL(SIOC) -> 22(ESP32)
// RESET          -> 3.3V
// PWDN		        -> GND
// HREF		        -> NC
const camera_config_t cam_conf = {
    .D0 = 36,
    .D1 = 39,
    .D2 = 34,
    .D3 = 35,
    .D4 = 32,
    .D5 = 33,
    .D6 = 25,
    .D7 = 26,
    .XCLK = 27,
    .PCLK = 14,
    .VSYNC = 13,
    .xclk_freq_hz = 10000000, // XCLK 10MHz
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0};

CameraManager::CameraManager()
{
  this->cam = new OV7670();
}

CameraManager::~CameraManager()
{
  delete this->cam;
  delete[] this->buf;
}

void CameraManager::init()
{
  Serial.println("[CameraManager] Initializing camera...");
  Wire.begin();
  Wire.setClock(400000);
  esp_err_t err = this->cam->init(&cam_conf, MODE, COLOR);
  if (err != ESP_OK)
  {
    Serial.print("[CameraManager] cam.init ERROR:");
    Serial.println(err);
    return;
  }
  this->cam->setPCLK(2, DBLV_CLK_x4);
  this->cam->vflip(false);
  Serial.printf("[CameraManager] cam MID = %X\n\r", this->cam->getMID());
  Serial.printf("[CameraManager] cam PID = %X\n\r", this->cam->getPID());

  switch (MODE)
  {
  case QVGA:
    this->w = 320;
    this->h = 240;
    break;
  case QQVGA:
    this->w = 160;
    this->h = 120;
    break;
  case QCIF:
    this->w = 176;
    this->h = 144;
    break;
  case QQCIF:
    this->w = 88;
    this->h = 72;
  }
  Serial.println("[CameraManager] Camera initialized");
}

void CameraManager::takePhoto()
{
  for (uint16_t y = 0; y < this->h; y++) {
    this->buf = this->cam->getLine(y + 1);
    Serial.print("[CameraManager] Photo line size:");
    Serial.println(sizeof(this->buf));
    //tft.drawRGBBitmap(0, y, buf, w, 1);
  }
}
