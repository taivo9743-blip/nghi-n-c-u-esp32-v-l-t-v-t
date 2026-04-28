#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCL 12
#define TFT_SDA 11
#define TFT_RES 10
#define TFT_DC   9
#define TFT_CS  13 
#define TFT_BL  21

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RES);
GFXcanvas16 *canvas;

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);
  canvas->fillScreen(ST77XX_BLACK);
}

void drawParticleTrace() {
  int x = random(0, 320);
  int y = random(0, 170);
  int len = random(5, 40);
  float angle = random(0, 360) * (PI / 180.0);
  
  // Vẽ vệt hạt với hiệu ứng mờ dần
  for (int i = 0; i < len; i++) {
    int px = x + cos(angle) * i;
    int py = y + sin(angle) * i;
    
    if (px >= 0 && px < 320 && py >= 0 && py < 170) {
      uint16_t color = tft.color565(255 - (i * 5), 255 - (i * 2), 255);
      canvas->drawPixel(px, py, color);
      // Đôi khi hạt bị tán xạ (đổi hướng nhẹ)
      angle += (random(-10, 10) * (PI / 180.0));
    }
  }
}

void loop() {
  // Hiệu ứng "phân rã": làm mờ dần các vệt cũ sau mỗi khung hình
  // Vì GFXcanvas không có alpha, ta giả lập bằng cách vẽ các điểm đen ngẫu nhiên
  for(int i=0; i<500; i++) {
    canvas->drawPixel(random(320), random(170), ST77XX_BLACK);
  }

  // Tỷ lệ xuất hiện hạt ngẫu nhiên (như thực tế)
  if (random(0, 100) > 95) {
    drawParticleTrace();
  }

  // Hiển thị thông tin hệ thống mờ ảo
  canvas->setTextSize(1);
  canvas->setTextColor(0x1082); // Màu xám cực tối
  canvas->setCursor(10, 155);
  canvas->print("PARTICLE DETECTOR MODE | S3-CORE | TAI");

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
  delay(16); // ~60 FPS
}