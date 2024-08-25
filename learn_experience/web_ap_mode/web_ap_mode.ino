#include <WiFi.h>
#define LED 12

//设置自身局域网的账号密码
char *ssid = "wed_learn_test";
char *password = "hello";

//用于表示连接的设备数
int account_num = 0;

int num = 1;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED, OUTPUT);
  Serial.begin(115200);
  WiFi.mode(WIFI_AP); //设置为AP模式
  WiFi.softAP(ssid, password);  //开启局域网
  digitalWrite(LED, HIGH);
  Serial.println("WIFI创建接入点成功");
  Serial.printf("WIFI名称: %s \r\n", WiFi.softAPSSID());  //打印局域网账户名称
  Serial.println(WiFi.softAPIP());  //打印IP地址
  Serial.println(WiFi.softAPmacAddress());  //打印设备物理地址（mac地址）

}

void loop() {
  // put your main code here, to run repeatedly:
  account_num = WiFi.softAPgetStationNum(); //获得连接设备数
  if(num == 1)  Serial.printf("%d\n", account_num);
  delay(5000)
}
