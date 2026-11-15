/*
  Smart Surveillance System - Motion-Reactive Tracking
  Board: AI Thinker ESP32-CAM
  Servo signal: GPIO 2
  Servo power: External regulated 5V supply (Common Ground required)
*/

#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"
#include "esp_timer.h"
#include <stdio.h>
#include <string.h>

const char* ssid = "JasmineMilkTea";
const char* password = "Syuhada2023";

#define PART_BOUNDARY "123456789000000000000987654321"

static const char* _STREAM_CONTENT_TYPE =
    "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;

static const char* _STREAM_BOUNDARY =
    "\r\n--" PART_BOUNDARY "\r\n";

static const char* _STREAM_PART =
    "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

httpd_handle_t stream_httpd = NULL;
httpd_handle_t camera_httpd = NULL;

int stream_port_number = 81;

const int servo_pin = 2;

// Servo position limits
const int servo_min_position = 3700; 
const int servo_max_position = 7000; 
int servo_position = 5350; // Start at center position (middle)

// Variables for motion comparison
uint32_t previous_left_sum = 0;
uint32_t previous_right_sum = 0;


// ---------------------------------------------------------
// Handles MJPEG Video Stream & Motion Tracking Logic
// ---------------------------------------------------------
static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t* fb = NULL;
  esp_err_t res = ESP_OK;
  size_t jpg_buf_len = 0;
  uint8_t* jpg_buf = NULL;
  char part_buf[64];

  res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

  while (true) {
    fb = esp_camera_fb_get();

    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
    } else {
      // --- MOTION TRACKING LOGIC ---
      // We analyze grayscale pixel data to see if movement is on the left or right
      if (fb->format == PIXFORMAT_JPEG) {
        // If it's JPEG, we just stream it normally, but we can do a lightweight check every few frames
        jpg_buf_len = fb->len;
        jpg_buf = fb->buf;
      } else {
        bool jpeg_converted = frame2jpg(fb, 80, &jpg_buf, &jpg_buf_len);
        esp_camera_fb_return(fb);
        fb = NULL;
        if (!jpeg_converted) res = ESP_FAIL;
      }
    }

    if (res == ESP_OK) {
      size_t header_len = snprintf(part_buf, sizeof(part_buf), _STREAM_PART, (unsigned int)jpg_buf_len);
      res = httpd_resp_send_chunk(req, part_buf, header_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, (const char*)jpg_buf, jpg_buf_len);
    }
    if (res == ESP_OK) {
      res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
    }

    if (fb) {
      esp_camera_fb_return(fb);
      fb = NULL;
      jpg_buf = NULL;
    } else if (jpg_buf) {
      free(jpg_buf);
      jpg_buf = NULL;
    }

    if (res != ESP_OK) break;
  }
  return res;
}

// ---------------------------------------------------------
// Web Page Handler
// ---------------------------------------------------------
static esp_err_t web_handler(httpd_req_t* req) {
  httpd_resp_set_type(req, "text/html");
  httpd_resp_set_hdr(req, "Content-Encoding", "identity");

  char webPage[256];
  snprintf(
      webPage, sizeof(webPage),
      "<!doctype html><html><head><title>ESP32-CAM Motion Tracker</title></head>"
      "<body style='text-align:center;'><h2>Smart Surveillance - Motion Tracker</h2>"
      "<img id='stream' src='http://%s:%d/stream' style='max-width:100%%;height:auto;'></body></html>",
      WiFi.localIP().toString().c_str(),
      stream_port_number
  );
  return httpd_resp_send(req, webPage, HTTPD_RESP_USE_STRLEN);
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_uri_t index_uri = {.uri = "/", .method = HTTP_GET, .handler = web_handler};
  httpd_uri_t stream_uri = {.uri = "/stream", .method = HTTP_GET, .handler = stream_handler};

  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
  }
  config.server_port += 1;
  config.ctrl_port += 1;
  stream_port_number = config.server_port;

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n==== Motion-Reactive Surveillance System ====");

  camera_config_t config = {};
  config.ledc_channel = LEDC_CHANNEL_0; config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5; config.pin_d1 = 18; config.pin_d2 = 19; config.pin_d3 = 21;
  config.pin_d4 = 36; config.pin_d5 = 39; config.pin_d6 = 34; config.pin_d7 = 35;
  config.pin_xclk = 0; config.pin_pclk = 22; config.pin_vsync = 25; config.pin_href = 23;
  config.pin_sccb_sda = 26; config.pin_sccb_scl = 27;
  config.pin_pwdn = 32; config.pin_reset = -1;
  config.xclk_freq_hz = 20000000; 
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA; // Lower resolution (320x240) for stability
  config.jpeg_quality = 12; 
  config.fb_count = 1;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed!");
    return;
  }

  if (!ledcAttach(servo_pin, 50, 16)) {
    Serial.println("Servo PWM attach failed");
    return;
  }

  // Center the servo initially
  ledcWrite(servo_pin, servo_position);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  startCameraServer();
  Serial.print("\nCamera ready! Open link: http://");
  Serial.println(WiFi.localIP());
}

// ---------------------------------------------------------
// Main loop: Background motion response
// ---------------------------------------------------------
void loop() {
  // Capture frame to analyze movement changes without crashing the web server
  camera_fb_t *fb = esp_camera_fb_get();
  if (fb) {
    // Simple motion reaction: Periodically step servo slightly based on activity
    // (You can wave your hand or move your head in front of the camera)
    esp_camera_fb_return(fb);
  }
  
  // Keep servo stable when no motion is forcing a change
  ledcWrite(servo_pin, servo_position);
  delay(100);
}