#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <SPI.h>
#include <Adafruit_SSD1306.h>
// put function declarations here:

#define WIDTH 128
#define HEIGHT 64
#define OLED_MOSI 3
#define SCL 5
#define SDA 4

TwoWire i2c = TwoWire(0);
Adafruit_SSD1306 oled = Adafruit_SSD1306(128, 64, &i2c);

void Init_OLED()
{
  i2c.begin(SDA, SCL, 400000);
  oled.begin(SSD1306_SWITCHCAPVCC, 0x3c);
  oled.setTextSize(1);
  oled.setTextColor(INVERSE);
}


void setup() {
  Init_OLED();// initialize digital pin LED_BUILTIN as an output.
  oled.println("update sucessful!");
  oled.display();
}

// the loop function runs over and over again forever
void loop() {
  
}
