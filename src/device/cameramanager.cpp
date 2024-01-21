#include "cameramanager.h"
#include <esp_camera.h>
#include <Arduino.h>

#define CAM_PIN_PWDN -1  // power down is not used
#define CAM_PIN_RESET -1 // software reset will be performed
#define CAM_PIN_XCLK 27
#define CAM_PIN_SIOD 21
#define CAM_PIN_SIOC 22

#define CAM_PIN_D7 26
#define CAM_PIN_D6 25
#define CAM_PIN_D5 33
#define CAM_PIN_D4 32
#define CAM_PIN_D3 35
#define CAM_PIN_D2 34
#define CAM_PIN_D1 39
#define CAM_PIN_D0 36
#define CAM_PIN_VSYNC 13
#define CAM_PIN_HREF -1
#define CAM_PIN_PCLK 14

CameraManager::CameraManager()
{
}

CameraManager::~CameraManager()
{
}

void CameraManager::init()
{
  Serial.println("[CameraManager] Initializing camera...");
  camera_config_t camera_config = {
      .pin_pwdn = CAM_PIN_PWDN,
      .pin_reset = CAM_PIN_RESET,
      .pin_xclk = CAM_PIN_XCLK,
      .pin_sccb_sda = CAM_PIN_SIOD,
      .pin_sccb_scl = CAM_PIN_SIOC,

      .pin_d7 = CAM_PIN_D7,
      .pin_d6 = CAM_PIN_D6,
      .pin_d5 = CAM_PIN_D5,
      .pin_d4 = CAM_PIN_D4,
      .pin_d3 = CAM_PIN_D3,
      .pin_d2 = CAM_PIN_D2,
      .pin_d1 = CAM_PIN_D1,
      .pin_d0 = CAM_PIN_D0,
      .pin_vsync = CAM_PIN_VSYNC,
      .pin_href = CAM_PIN_HREF,
      .pin_pclk = CAM_PIN_PCLK,

      .xclk_freq_hz = 10000000, // EXPERIMENTAL: Set to 16MHz on ESP32-S2 or ESP32-S3 to enable EDMA mode
      .ledc_timer = LEDC_TIMER_0,
      .ledc_channel = LEDC_CHANNEL_0,

      .pixel_format = PIXFORMAT_RGB565, // YUV422,GRAYSCALE,RGB565,JPEG
      .frame_size = FRAMESIZE_QVGA,     // QQVGA-UXGA, For ESP32, do not use sizes above QVGA when not JPEG. The performance of the ESP32-S series has improved a lot, but JPEG mode always gives better frame rates.

      .jpeg_quality = 30,                 // 0-63, for OV series camera sensors, lower number means higher quality
      .fb_count = 1,                      // When jpeg mode is used, if fb_count more than one, the driver will work in continuous mode.
      .grab_mode = CAMERA_GRAB_WHEN_EMPTY // CAMERA_GRAB_LATEST. Sets when buffers should be filled
  };
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK)
  {
    Serial.print("[CameraManager] Camera Init Failed: ");
    Serial.print(err);
    return;
  }
  Serial.println("[CameraManager] Camera initialized");
}

void CameraManager::takePhoto()
{
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb)
  {
    Serial.println("[CameraManager] Camera Capture Failed");
    return;
  }
  Serial.print("[CameraManager] Taken photo. Length: ");
  Serial.println(fb->len);
  esp_camera_fb_return(fb);
}
