#include "esp_camera.h"
#include <WiFi.h>

// ================= WIFI =================
const char* ssid = "narzo 50A";
const char* password = "12345678";

// ================= CAMERA MODEL =================
#define CAMERA_MODEL_AI_THINKER   // ✅ FIXED

#include "camera_pins.h"

// ================= SERVER =================
WiFiServer server(80);

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  Serial.println();

  camera_config_t config;

  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;

  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;

  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;

  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;

  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;

  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  // 🔥 OPTIMIZED SETTINGS
  config.frame_size = FRAMESIZE_QQVGA;   // fast & smooth
  config.jpeg_quality = 20;              // compressed
  config.fb_count = 1;                   // stable
  config.grab_mode = CAMERA_GRAB_LATEST; // no lag

  // ================= INIT CAMERA =================
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("❌ Camera init failed");
    return;
  }

  Serial.println("✅ Camera init success");

  // ================= WIFI =================
  WiFi.begin(ssid, password);
  WiFi.setSleep(false);

  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi Connected");
  Serial.print("Stream URL: http://");
  Serial.println(WiFi.localIP());

  server.begin();
}

// ================= LOOP =================
void loop() {

  WiFiClient client = server.available();

  if (!client) return;

  Serial.println("Client connected");

  // MJPEG stream header
  client.print(
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: multipart/x-mixed-replace; boundary=frame\r\n\r\n"
  );

  while (client.connected()) {

    camera_fb_t *fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Frame capture failed");
      continue;
    }

    client.print("--frame\r\n");
    client.print("Content-Type: image/jpeg\r\n\r\n");
    client.write(fb->buf, fb->len);
    client.print("\r\n");

    esp_camera_fb_return(fb);

    delay(10);  // 🔥 smooth FPS
  }

  client.stop();
  Serial.println("Client disconnected");
}
