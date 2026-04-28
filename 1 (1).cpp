#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCL 12
#define TFT_SDA 11
#define TFT_RES 10
#define TFT_DC   9
#define TFT_CS  13 
#define TFT_BL  21
#define BUTTON_PIN 0

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RES);
GFXcanvas16 *canvas;
int glitchMode = 0;

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);
}

void loop() {
  uint16_t* buffer = canvas->getBuffer();

  if (digitalRead(BUTTON_PIN) == LOW) {
    glitchMode = (glitchMode + 1) % 4;
    canvas->fillScreen(ST77XX_BLACK);
    delay(300);
  }

  static uint32_t frame = 0;
  frame++;

  for (int i = 0; i < 320 * 170; i++) {
    int x = i % 320;
    int y = i / 320;

    switch (glitchMode) {
      case 0: // Nhiễu hạt "Bitwise XOR"
        buffer[i] = (x ^ y ^ frame) * 0x1234;
        break;
      case 1: // Dải màu "Feedback"
        buffer[i] = (x * frame) ^ (y + frame);
        break;
      case 2: // "Sọc sóng" kỹ thuật số
        buffer[i] = (uint16_t)(sin(x * 0.1 + frame * 0.5) * 0xFFFF) & (y * 0xFF);
        break;
      case 3: // "Ma trận lỗi"
        if (random(100) > 98) buffer[i] = 0xFFFF;
        else if (random(100) > 95) buffer[i] = 0x07E0;
        else buffer[i] &= 0xFE00;
        break;
    }
  }

  // Chèn một vài thanh bar "lỗi" ngẫu nhiên
  if (frame % 5 == 0) {
    canvas->fillRect(0, random(170), 320, random(2, 10), random(0xFFFF));
  }

  // Text overlay phong cách hacker
  canvas->setTextSize(1);
  canvas->setTextColor(ST77XX_WHITE);
  canvas->setCursor(10, 155);
  canvas->printf("CORRUPTION_LEVEL: %d | MODE: %d | OPERATOR: TAI", random(10, 99), glitchMode);

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
}