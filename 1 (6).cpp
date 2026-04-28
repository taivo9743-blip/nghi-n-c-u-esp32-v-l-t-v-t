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

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  
  randomSeed(analogRead(0));
  drawNewTree();
}

void drawBranch(float x, float y, float angle, float length, int depth) {
  if (depth == 0) return;

  // Tính toán điểm kết thúc của cành
  float x2 = x + cos(angle * DEG_TO_RAD) * length;
  float y2 = y + sin(angle * DEG_TO_RAD) * length;

  // Màu sắc đổi từ nâu sang xanh lá (từ gốc lên ngọn)
  uint16_t color = (depth > 2) ? tft.color565(139, 69, 19) : tft.color565(34, 139, 34);
  
  tft.drawLine(x, y, x2, y2, color);

  // Vẽ các cành con với góc và độ dài ngẫu nhiên một chút
  drawBranch(x2, y2, angle - random(15, 35), length * 0.75, depth - 1);
  drawBranch(x2, y2, angle + random(15, 35), length * 0.75, depth - 1);
}

void drawNewTree() {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(0x7BEF);
  tft.setCursor(10, 10);
  tft.print("GENERATING DIGITAL BONSAI...");
  
  // Bắt đầu vẽ từ giữa đáy màn hình, hướng lên trên (-90 độ)
  drawBranch(160, 165, -90, 40, 8);
  
  tft.setCursor(10, 155);
  tft.print("ARTIST: S3-ALGORITHM | OWNER: TAI");
}

void loop() {
  // Nhấn nút BOOT để trồng một cái cây mới
  if (digitalRead(BUTTON_PIN) == LOW) {
    drawNewTree();
    delay(500); 
  }
}