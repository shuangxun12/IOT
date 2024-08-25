#include "esp_camera.h"
#include "FS.h"
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"


#include <WiFi.h>
#include <WebServer.h>
#include "HTTPClient.h"
#include <ESP32Servo.h>
#include "esp_http_server.h" //用于创建和管理http服务，实现web服务器功能



//宏定义引脚，用于初始化摄像头
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

char* ssid = "learn";
char* password = "cw521997";

httpd_handle_t stream_httpd = NULL; //定义，后续用于服务器回发相片

unsigned int picture_num = 0;

//定义摄像头
camera_config_t config;

//扫描wifi
void Scan_Useful_WiFi()
{
  int num = WiFi.scanNetworks();
  Serial.printf("扫描到 %d 个可用WiFi\r\n", num);
  for(int i = 0; i < num; i++)
  {
    Serial.print(WiFi.SSID(i));
    Serial.printf("[%d] \r\n", WiFi.RSSI(i));
  }
}

//初始化WiFi并连接
void WiFiInit()
{
  WiFi.mode(WIFI_STA);
  Scan_Useful_WiFi();
  WiFi.begin(ssid, password);
  Serial.printf("开始连接wifi: %s", ssid);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(". ");
    delay(200);
  }

  Serial.println();
  Serial.println("WiFi连接成功");
  Serial.print("IP地址:");
  Serial.println(WiFi.localIP());
}


//初始化摄像头
void ConfigInitCamera()
{
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
  config.pixel_format = PIXFORMAT_JPEG; //YUV422,GRAYSCALE,RGB565,JPEG

  // Select lower framesize if the camera doesn't support PSRAM
  if(psramFound()){
    config.frame_size = FRAMESIZE_UXGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 10; //10-63 lower number means higher quality
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // 初始化摄像头
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  //摄像头具体的配置（更改影响回传的照片）
  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  Serial.println("初始化成功");
}



static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;  // 指向摄像头帧缓冲区的指针
    esp_err_t res = ESP_OK;   // 函数执行结果，默认为成功

    // 从摄像头获取一张图像帧
    fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("Camera capture failed");  // 摄像头捕获失败
        return ESP_FAIL;
    }

    // 设置 HTTP 响应类型为 JPEG 图像
    res = httpd_resp_set_type(req, "image/jpeg");
    if (res != ESP_OK) {
        esp_camera_fb_return(fb);  // 释放图像帧缓冲区
        Serial.println("Failed to set response type");
        return res;
    }

    // 发送 JPEG 图像数据到客户端
    res = httpd_resp_send(req, (const char *)fb->buf, fb->len);
    if (res != ESP_OK) {
        Serial.println("Failed to send image data");  // 发送图像数据失败
    }

    // 释放图像帧缓冲区
    esp_camera_fb_return(fb);

    return res;  // 返回函数执行结果
}


void startCameraServer()
{
  //创建httpd的配置
  httpd_config_t server_mine = HTTPD_DEFAULT_CONFIG();
  //设置端口号
  server_mine.server_port = 80;
  
  //
  httpd_uri_t index_uri = {
    .uri      = "/",
    .method   = HTTP_GET,
    .handler  = stream_handler,
    .user_ctx = NULL
  };   

  //启动httpd服务器
  if(httpd_start(&stream_httpd, &server_mine) == ESP_OK){
    httpd_register_uri_handler(stream_httpd, &index_uri);
  }

}


void setup() {
  // put your setup code here, to run once:
  WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);//禁用输出监视
  

  //开启串口监视
  Serial.begin(115200);
  
  //连接wifi
  WiFiInit();

  //相机初始化
  Serial.println("Initailizing the camera module...");
  ConfigInitCamera(); 
  startCameraServer();

}

void loop() {
  // put your main code here, to run repeatedly:
}
