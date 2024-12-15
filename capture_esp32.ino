/**********************************************************************************
 *  TITLE: Motion Sensor Security Camera using ESP32-CAM & Blynk
 *  Click on the following links to learn more. 
 *  YouTube Video: https://youtu.be/LqX9EMFSoDA
 *  Related Blog : https://easyelectronicsproject.com/esp32-projects/
 *  by Tech StudyCell
 **********************************************************************************/

 /**********************************************************************************
 *  Preferences--> Aditional boards Manager URLs : https://dl.espressif.com/dl/package_esp32_index.json, http://arduino.esp8266.com/stable/package_esp8266com_index.json
 *  Board Settings:
 *  Board: "ESP32 Wrover Module"
 *  Upload Speed: "921600"
 *  Flash Frequency: "80MHz"
 *  Flash Mode: "QIO"
 *  Partition Scheme: "Hue APP (3MB No OTA/1MB SPIFFS)"
 *  Core Debug Level: "None"
 *  COM Port: Depends *On Your System*
 *  
 *  GPIO 0 must be connected to GND pin while uploading the sketch
 *  After connecting GPIO 0 to GND pin, press the ESP32 CAM on-board RESET button to put the board in flashing mode
 ***************************************************************************************/
// Select camera model
#define BLYNK_TEMPLATE_ID "TMPL3w6IIxiPW"
#define BLYNK_TEMPLATE_NAME "Esp32 cam security system"
#define BLYNK_AUTH_TOKEN "POdofEl1498BspGZJsZXR3mLNt1bBBQv"
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
#include "esp_camera.h"
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//


#include "camera_pins.h"

#define PIR 14
#define PHOTO 13
#define LED 4

const char* ssid = "Iot 8";
const char* password = "rpw1@iot";
char auth[] = "POdofEl1498BspGZJsZXR3mLNt1bBBQv";  //sent by Blynk

String local_IP;

void startCameraServer();

void takePhoto()
{
  digitalWrite(LED, HIGH);
  delay(200);
  uint32_t randomNum = random(50000);
  Serial.println("http://"+local_IP+"/capture?_cb="+ (String)randomNum);
  String imageUrl = ("https://"+local_IP+"/image.jpg");
  Blynk.virtualWrite(V0, imageUrl);
  Blynk.setProperty(V0, "urls", "http://"+local_IP+"/capture?_cb="+(String)randomNum);
  digitalWrite(LED, LOW);
  delay(1000);
}

void setup() {
  Serial.begin(115200);
  pinMode(LED,OUTPUT);
  Serial.setDebugOutput(true);
  Serial.println();
  pinMode(PIR, INPUT);
  
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
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // if PSRAM IC present, init with UXGA resolution and higher JPEG quality
  //                      for larger pre-allocated frame buffer.
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA;
    config.jpeg_quality = 10;
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  // camera init
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  // initial sensors are flipped vertically and colors are a bit saturated
  if (s->id.PID == OV3660_PID) {
    s->set_vflip(s, 1); // flip it back
    s->set_brightness(s, 1); // up the brightness just a bit
    s->set_saturation(s, -2); // lower the saturation
  }
  // drop down frame size for higher initial frame rate
  s->set_framesize(s, FRAMESIZE_QVGA);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  startCameraServer();

  Serial.print("Camera Ready! Use 'http://");
  Serial.print(WiFi.localIP());
  local_IP = WiFi.localIP().toString();
  Serial.println("' to connect");
  Blynk.begin(auth, ssid, password);
}

void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  int alert=digitalRead(PIR);
  if(alert== 1){
  Serial.println("Send Notification");
  Blynk.logEvent("alert_thieft", "Motion detected at home!");
  Serial.println("Capture Photo");
  takePhoto();
  delay(3000);
  }
  if(digitalRead(PHOTO) == HIGH){
  //Blynk.logEvent("alert_thieft", "Motion detected at home!");
  Serial.println("Capture Photo");
  takePhoto();
  }  
}
