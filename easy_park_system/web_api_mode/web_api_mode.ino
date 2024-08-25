#include <WiFi.h>
#include <WebServer.h>
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include <Adafruit_SSD1306.h>
#include <string.h>

//定义控制SG90舵机的频率、管道、分辨率、以及控制引脚
#define FREQ 50 //频率
#define CHANNEL 0 //通道
#define RESOLUTION 10 //分辨率
#define LED 3 // led 引脚

//定义OLED屏幕
#define WIDTH 128 //长度
#define HEIGHT 64  //宽度
#define OLED_MOSI 3
#define SCL 5  //时钟线
#define SDA 4  //数据线

//取模汉字（十六进制）
const unsigned char characters [][32] = 
{
  {0x10,0x04,0x10,0x84,0x10,0x84,0x10,0x84,0x10,0x84,0x10,0x84,0x10,0x84,0x10,0x84,
  0x10,0x84,0x10,0x84,0x10,0x84,0x10,0x84,0x20,0x84,0x20,0x84,0x40,0x04,0x80,0x04},/*"川",0*/
};

TwoWire i2c = TwoWire(0);
Adafruit_SSD1306 oled = Adafruit_SSD1306(128, 64, &i2c);

//设置外部WiFi的账号密码
char* ssid = "learn";
char* password = "cw521997";

//定义用于接收来自主机传输的数据的全局变量
String car_number;
int right;

//定义服务器
WebServer server(80);

//初始化OLED屏幕
void Init_OLED()
{
  i2c.begin(SDA, SCL, 400000);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  oled.setTextSize(1);
  oled.setTextColor(INVERSE);
}

//显示车辆识别系统初始画面
void OLED_first()
{
  oled.clearDisplay();
  oled.print("hello world!\n");
  oled.display();
}

//打印接收来自主机的数据
void OLED_infoprint()
{
  oled.clearDisplay();
  oled.drawBitmap(0, 0, characters[0], 16, 16, 1);
  oled.setCursor(20, 0);
  for(int i = 1; i <= 6; i++)
    oled.printf("%c", car_number[i]);
  oled.display();
  delay(500);
  if(right == 1)
  {
    oled.setCursor(20, 20);
    oled.println("Welcome!!!");
    oled.display();
  }
  else
  {
    oled.setCursor(20, 20);
    oled.println("Sorry '_'");
    oled.display();
  }
  delay(500);
}

//初始化SG90舵机
void SG90_init()
{
  ledcSetup(CHANNEL, FREQ, RESOLUTION);
  ledcAttachPin(LED, CHANNEL);
}

//根据主机传来的数据决定是否启动摇杆
void SG90_move()
{
  if(right == 1)
  {
    ledcWrite(CHANNEL, 50);
    delay(200);
    ledcWrite(CHANNEL, 0);
    delay(5000);
    ledcWrite(CHANNEL, 100);
    delay(250);
    ledcWrite(CHANNEL, 0);
  }
}

//用于解析来自主机发送的json数据，并存入对应的变量中
void JsonSolution(String data)
{
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, data);
  car_number = doc["car_number"].as<String>();
  right = doc["right"].as<int>();
  Serial.printf("%s   %d\n", car_number, right);
}

//扫描可用wifi并于串口打印
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

//用于处理来自主机的post请求
void handleRoot()
{
  String data = server.arg("data");
  Serial.println(data);
  server.send(200,"text/html",data);
  Serial.println("The data has received");
  JsonSolution(data);
  
  OLED_infoprint();
  SG90_move();
  OLED_first();
}


void setup() {
  // put your setup code here, to run once:
  SG90_init();
  Init_OLED();
  OLED_first();
  Serial.begin(115200);

  //设置为STA模式
  WiFi.mode(WIFI_STA);

  //扫描并连接WiFi
  Scan_Useful_WiFi();
  WiFi.begin(ssid, password);
  Serial.printf("开始连接wifi: %s", ssid);
  while(WiFi.status() != WL_CONNECTED)
  {
    Serial.print(". ");
    delay(200);
  }

  //打印IP地址
  Serial.println();
  Serial.println("WiFi连接成功");
  Serial.print("IP地址:");
  Serial.println(WiFi.localIP());

  //启动web服务器
  server.on("/",HTTP_POST, handleRoot);
  Serial.println("成功启动服务器");
  server.begin();

}
void loop() {
  // put your main code here, to run repeatedly:
  server.handleClient();//循环等待主机发送数据
}
