// #include <ESP32Servo.h>
// //使用库<ESP32Servo.h>，调用函数来控制舵机;
// Servo servo;
// int pos = 0;//全局变量，用来控制舵机的角度;

// void setup()
// {
//   ESP32PWM::allocateTimer(1); //为舵机配置一个定时器（括号里的“1”是指一个定时器）;
//   servo.setPeriodHertz(50); //设置频率;
//   servo.attach(3, 500, 2500); //设置舵机最小脉冲宽度为500微秒， 最大为2500微秒, 同时绑定3号chip;
// }

// void loop()
// {
//   for(pos = 0; pos <= 180; pos++)
//   {
//     servo.write(pos);
//     delay(20);
//   }
//   for(pos = 180; pos >= 0; pos--)
//   {
//     servo.write(pos);
//     delay(20);
//   }
// }























//使用pwm手动调节
#define FREQ 50 //频率
#define CHANNEL 0 //通道
#define RESOLUTION 10 //分辨率
#define LED 3 // led 引脚
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  ledcSetup(CHANNEL, FREQ, RESOLUTION);
  ledcAttachPin(LED, CHANNEL);
}

// the loop function runs over and over again forever
void loop() {
  ledcWrite(CHANNEL, 50);
  delay(500);
  ledcWrite(CHANNEL, 0);
  delay(1000);
  ledcWrite(CHANNEL, 100);
  delay(500);
}
