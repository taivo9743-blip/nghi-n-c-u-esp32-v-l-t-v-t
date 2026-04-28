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

#define MAX_PARTICLES 100
struct Particle {
  float x, y, vx, vy;
  uint16_t color;
};

Particle p[MAX_PARTICLES];

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);

  for(int i=0; i<MAX_PARTICLES; i++) {
    p[i] = {(float)random(320), (float)random(170), 0, 0, 
            tft.color565(random(50,255), random(150,255), 255)};
  }
}

void loop() {
  // Không xóa toàn bộ màn hình mà vẽ đè màu đen mờ để tạo hiệu ứng đuôi (Motion Blur)
  // Tuy nhiên GFXcanvas không hỗ trợ alpha, nên mình xóa nhanh bằng cách vẽ lại các hạt cũ
  canvas->fillScreen(ST77XX_BLACK);

  float targetX = 160;
  float targetY = 85;
  bool pushing = (digitalRead(BUTTON_PIN) == LOW);

  for(int i=0; i<MAX_PARTICLES; i++) {
    float dx = targetX - p[i].x;
    float dy = targetY - p[i].y;
    float dist = sqrt(dx*dx + dy*dy);
    if(dist < 1) dist = 1;

    // Lực hấp dẫn hoặc lực đẩy
    float force = pushing ? -2.0 : 0.5;
    p[i].vx += (dx / dist) * force;
    p[i].vy += (dy / dist) * force;

    // Ma sát để hạt không bay quá nhanh
    p[i].vx *= 0.96;
    p[i].vy *= 0.96;

    p[i].x += p[i].vx;
    p[i].y += p[i].vy;

    // Giới hạn biên màn hình
    if(p[i].x < 0 || p[i].x > 320) p[i].vx *= -1;
    if(p[i].y < 0 || p[i].y > 170) p[i].vy *= -1;

    // Vẽ hạt với kích thước biến thiên theo vận tốc
    float speed = sqrt(p[i].vx*p[i].vx + p[i].vy*p[i].vy);
    int size = map(speed, 0, 10, 1, 4);
    canvas->fillCircle(p[i].x, p[i].y, size, p[i].color);
  }

  // UI
  canvas->setTextSize(1);
  canvas->setTextColor(0x52AA);
  canvas->setCursor(10, 10);
  canvas->printf("GRAVITY CORE: %s", pushing ? "REPELLING" : "ATTRACTING");
  canvas->setCursor(220, 155);
  canvas->print("BY: CONG TAI");

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
}