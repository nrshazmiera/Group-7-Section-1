#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "img_converters.h"
#include "camera_index.h"
#include "Arduino.h"
#include <list>
#include "human_face_detect_msr01.hpp"
#include "human_face_detect_mnp01.hpp"

extern void onFaceDetected();
extern void onFaceLost();
extern volatile bool faceCurrentlyDetected;

static httpd_handle_t camera_httpd = NULL;
static httpd_handle_t stream_httpd = NULL;

#define STREAM_BOUNDARY "123456789000000000000987654321"
static const char *STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" STREAM_BOUNDARY;
static const char *STREAM_BOUNDARY_TEXT = "\r\n--" STREAM_BOUNDARY "\r\n";
static const char *STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

static esp_err_t index_handler(httpd_req_t *req) {
  httpd_resp_set_type(req, "text/html; charset=utf-8");
  return httpd_resp_send(req, INDEX_HTML, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t status_handler(httpd_req_t *req) {
  char json[64];
  snprintf(json, sizeof(json), "{\"face_detected\":%s}",
           faceCurrentlyDetected ? "true" : "false");
  httpd_resp_set_type(req, "application/json");
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  return httpd_resp_send(req, json, HTTPD_RESP_USE_STRLEN);
}

static esp_err_t stream_handler(httpd_req_t *req) {
  esp_err_t res = httpd_resp_set_type(req, STREAM_CONTENT_TYPE);
  if (res != ESP_OK) return res;
  httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
  httpd_resp_set_hdr(req, "X-Framerate", "10");

  // Espressif's two-stage face detector. QVGA RGB565 keeps memory use manageable.
  HumanFaceDetectMSR01 detector1(0.1F, 0.5F, 10, 0.2F);
  HumanFaceDetectMNP01 detector2(0.5F, 0.3F, 5);
  bool previousFaceState = false;

  while (true) {
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      res = ESP_FAIL;
      break;
    }

    bool faceFound = false;
    if (fb->format == PIXFORMAT_RGB565) {
      std::list<dl::detect::result_t> &candidates = detector1.infer(
        (uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3});
      std::list<dl::detect::result_t> &results = detector2.infer(
        (uint16_t *)fb->buf, {(int)fb->height, (int)fb->width, 3}, candidates);
      faceFound = !results.empty();
    }

    faceCurrentlyDetected = faceFound;
    if (faceFound && !previousFaceState) {
      Serial.println("FACE DETECTED");
      onFaceDetected();
    } else if (!faceFound && previousFaceState) {
      Serial.println("FACE LOST");
      onFaceLost();
    }
    previousFaceState = faceFound;

    uint8_t *jpgBuf = NULL;
    size_t jpgLen = 0;
    bool converted = fmt2jpg(fb->buf, fb->len, fb->width, fb->height,
                             PIXFORMAT_RGB565, 80, &jpgBuf, &jpgLen);
    esp_camera_fb_return(fb);

    if (!converted || !jpgBuf) {
      Serial.println("JPEG conversion failed");
      res = ESP_FAIL;
      break;
    }

    char partHeader[64];
    int headerLen = snprintf(partHeader, sizeof(partHeader), STREAM_PART,
                             (unsigned int)jpgLen);
    res = httpd_resp_send_chunk(req, STREAM_BOUNDARY_TEXT,
                                strlen(STREAM_BOUNDARY_TEXT));
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, partHeader, headerLen);
    if (res == ESP_OK) res = httpd_resp_send_chunk(req, (const char *)jpgBuf, jpgLen);
    free(jpgBuf);

    if (res != ESP_OK) break;
    vTaskDelay(pdMS_TO_TICKS(80));
  }

  faceCurrentlyDetected = false;
  onFaceLost();
  return res;
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_uri_handlers = 8;
  config.server_port = 80;

  httpd_uri_t index_uri = {};
  index_uri.uri = "/";
  index_uri.method = HTTP_GET;
  index_uri.handler = index_handler;

  httpd_uri_t status_uri = {};
  status_uri.uri = "/status";
  status_uri.method = HTTP_GET;
  status_uri.handler = status_handler;

  if (httpd_start(&camera_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(camera_httpd, &index_uri);
    httpd_register_uri_handler(camera_httpd, &status_uri);
    Serial.println("Web page server started on port 80");
  } else {
    Serial.println("Failed to start web page server");
  }

  config.server_port = 81;
  config.ctrl_port = 32769;

  httpd_uri_t stream_uri = {};
  stream_uri.uri = "/stream";
  stream_uri.method = HTTP_GET;
  stream_uri.handler = stream_handler;

  if (httpd_start(&stream_httpd, &config) == ESP_OK) {
    httpd_register_uri_handler(stream_httpd, &stream_uri);
    Serial.println("Video stream server started on port 81");
  } else {
    Serial.println("Failed to start video stream server");
  }
}
