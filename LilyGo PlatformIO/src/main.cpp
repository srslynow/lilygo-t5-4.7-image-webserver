#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <SPI.h>

#include "epd_driver.h"

// Lilygo T5 4.7inch has a resolution of 960 x 540

const char* api_url = "http://192.168.50.2:12345/image";

const char* ssid     = "your wifi network name";
const char* password = "your password";

// how long we'll sleep (minutes)
long SleepDuration   = 60;
// timer to check how long we've been awake
long StartTime       = 0;
// save wifi signal strenght
int wifi_signal;

// declare framebuffer and (network) image buffer
// these'll be initialized into SPI-RAM in our setup function
uint8_t *framebuffer;
uint8_t *imagebuffer;
int image_width, image_height;

const char * headerKeys[] = {"Image-Width", "Image-Height"};
const size_t numberOfHeaders = 2;

uint8_t StartWiFi() {
  Serial.println("Connecting to: " + String(ssid));
  IPAddress dns(192, 168, 50, 2);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA); // switch off AP
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(500);
    WiFi.begin(ssid, password);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifi_signal = WiFi.RSSI(); // Get Wifi Signal strength now, because the WiFi will be turned off to save power!
    Serial.println("WiFi connected at: " + WiFi.localIP().toString());
  }
  else Serial.println("WiFi connection *** FAILED ***");
  return WiFi.status();
}

void StopWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi switched Off");
}

void draw_image()
{
    epd_poweron();
    epd_clear();
    epd_draw_grayscale_image(epd_full_screen(), framebuffer); 
    epd_poweroff();
}

void BeginSleep() {
  epd_poweroff_all();
  long SleepTimer = SleepDuration * 60; //Some ESP32 have a RTC that is too fast to maintain accurate time, so add an offset
  esp_sleep_enable_timer_wakeup(SleepTimer * 1000000LL); // in Secs, 1000000LL converts to Secs as unit = 1uSec
  Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
  Serial.println("Entering " + String(SleepTimer) + " (secs) of sleep time");
  Serial.println("Starting deep-sleep period...");
  esp_deep_sleep_start();  // Sleep for e.g. 30 minutes
}

void QueryServer()
{
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  http.begin(api_url); //HTTP
  http.collectHeaders(headerKeys, numberOfHeaders);
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  // httpCode will be negative on error
  if(httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      // get lenght of document (is -1 when Server sends no Content-Length header)
      int len = http.getSize();
      Serial.printf("[HTTP] Server says it'll send a response with %i bytes (excluding headers)\n", len);
      // we've made some custom HTTP Response headers which tell us what size the image was
      image_width = http.header("Image-Width").toInt();
      image_height = http.header("Image-Height").toInt();
      // get a handle to the tcp stream
      WiFiClient * stream = http.getStreamPtr();
      // keep track of how many bytes we've read thus far
      size_t read = 0;
      // read all data from server
      while(http.connected() && (len > 0 || len == -1))
      {
        // get available data size
        size_t size = stream->available();
        // if size > 0, read the bytes into 
        if(size) {
          // read http bytes progressively into our imagebuffer
          int c = stream->readBytes(imagebuffer + read, 2048);
          // update counters
          len -= c;
          read += c;
          }
      }
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  // close http connection
  http.end();
}


void setup()
{
    // start execution, print something to the serial out and start timing
    Serial.begin(115200);
    Serial.println("Wakey wakey!");
    StartTime = millis();
    // check SPIRAM support
    heap_caps_print_heap_info(MALLOC_CAP_SPIRAM);
    multi_heap_info_t info;
    heap_caps_get_info(&info, MALLOC_CAP_SPIRAM);
    Serial.printf("total PSRAM size = %lu\n", info.total_free_bytes + info.total_allocated_bytes );
    // initialize framebuffer space in SPIRAM
    framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    imagebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    memset(imagebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
    // initialize the screen
    epd_init();
    // allocate space to store our image data in SPI-RAM
    // we plot 4-bits per pixel, since we have to allocate per byte (8 bits) we'll allocate (half the number of pixels) in bytes
    // framebuffer = (uint8_t *)heap_caps_malloc(EPD_WIDTH * EPD_HEIGHT / 2, MALLOC_CAP_SPIRAM);
    // epd_copy_to_framebuffer(area, (uint8_t *) datastem_logo_data, framebuffer);
    
    // start WIFI
    if (StartWiFi() == WL_CONNECTED)
    {
      // wifi connected successfully, get the image from our server
      QueryServer();
      // image data resides in imagebuffer now, copy it over to the framebuffer
      Rect_t area = {
          .x = (EPD_WIDTH - image_width) / 2,
          .y = (EPD_HEIGHT - image_height) / 2,
          .width = image_width,
          .height =  image_height
      };
      epd_copy_to_framebuffer(area, imagebuffer, framebuffer);
      // do a fullscreen redraw with the framebuffer
      draw_image();
    }
    // we don't need the buffers anymore, free them
    free(framebuffer);
    free(imagebuffer);
    // start the sleeping procedure
    BeginSleep();
}


void loop()
{
    delay(3000);
}
