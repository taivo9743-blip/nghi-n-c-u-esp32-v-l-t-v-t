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

float platterAngle = 0;
float headPos = 40;     // Vị trí hiện tại của đầu đọc (bán kính)
float targetPos = 40;   // Vị trí cần đến
bool IsSeeking = false;

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
  canvas->fillScreen(0x1082); // Màu xám kim loại tối

  // 1. Vẽ phiến đĩa (Platter)
  int cx = 110, cy = 85;
  canvas->fillCircle(cx, cy, 75, 0x7BEF); // Viền đĩa
  canvas->fillCircle(cx, cy, 73, 0x2104); // Mặt đĩa
  
  // Vẽ các rãnh đĩa quay mờ mờ
  platterAngle += 15;
  for(int a=0; a<360; a+=45) {
    float rad = (a + platterAngle) * PI / 180.0;
    canvas->drawLine(cx, cy, cx + cos(rad)*70, cy + sin(rad)*70, 0x3186);
  }
  canvas->fillCircle(cx, cy, 10, 0x7BEF); // Trục đĩa

  // 2. Logic di chuyển đầu đọc
  if (digitalRead(BUTTON_PIN) == LOW) {
    targetPos = random(20, 70); // Yêu cầu dữ liệu mới
    IsSeeking = true;
  }

  // Di chuyển đầu đọc tới đích (Seeking)
  if (abs(headPos - targetPos) > 0.5) {
    headPos += (targetPos > headPos) ? 1.5 : -1.5;
  } else {
    IsSeeking = false;
  }

  // 3. Vẽ cánh tay đầu đọc (Actuator Arm)
  int armBaseX = 260, armBaseY = 85;
  // Tính toán góc của cánh tay dựa trên vị trí headPos trên đĩa
  float armAngle = map(headPos, 20, 70, 160, 200); 
  float radArm = armAngle * PI / 180.0;
  int hx = armBaseX + cos(radArm) * 160;
  int hy = armBaseY + sin(radArm) * 160;

  // Vẽ cánh tay
  canvas->drawLine(armBaseX, armBaseY, hx, hy, ST77XX_WHITE);
  canvas->fillCircle(hx, hy, 4, 0xF800); // Đầu đọc (Head)
  canvas->fillCircle(armBaseX, armBaseY, 15, 0x4208); // Trục cánh tay

  // 4. UI Thông tin
  canvas->setTextColor(ST77XX_CYAN);
  canvas->setCursor(210, 20);
  canvas->printf("STATUS: %s", IsSeeking ? "SEEKING" : "IDLE");
  canvas->setCursor(210, 35);
  canvas->printf("TRACK: %02.0f", headPos);
  
  canvas->setTextSize(1);
  canvas->setTextColor(0x52AA);
  canvas->setCursor(10, 155);
  canvas->print("HDD MECHANICAL SIM | ARCH: ESP32-S3 | TAI");

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
}