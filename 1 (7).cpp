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

#define TAPE_SIZE 40
int tape[TAPE_SIZE];
float headPos = 20.0;
int currentState = 0;

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);

  // Khởi tạo băng chuyền ngẫu nhiên
  for(int i=0; i<TAPE_SIZE; i++) tape[i] = random(2);
}

void loop() {
  canvas->fillScreen(ST77XX_BLACK);
  
  // Màu Amber (Hổ phách) cổ điển
  uint16_t amber = 0xFDA0; 

  // 1. Vẽ khung đầu đọc (Head) cố định ở giữa
  canvas->drawRect(145, 60, 30, 45, ST77XX_WHITE);
  canvas->drawRect(144, 59, 32, 47, amber);

  // 2. Cập nhật vị trí băng chuyền (chạy liên tục)
  static float offset = 0;
  offset += 0.05; // Tốc độ chạy
  if(offset >= 1.0) {
    offset = 0;
    // Khi một ô đi qua, thực hiện logic Turing đơn giản
    int headIdx = (int)headPos % TAPE_SIZE;
    if(tape[headIdx] == 0 && currentState == 0) { tape[headIdx] = 1; currentState = 1; }
    else if(tape[headIdx] == 1 && currentState == 1) { tape[headIdx] = 0; currentState = 0; }
  }

  // 3. Vẽ băng chuyền dữ liệu
  canvas->setTextSize(3);
  for(int i = -5; i < 6; i++) {
    int displayIdx = ((int)headPos + i + TAPE_SIZE) % TAPE_SIZE;
    int xPos = 150 + (i * 40) - (offset * 40);
    
    canvas->setTextColor(amber);
    canvas->setCursor(xPos, 70);
    canvas->print(tape[displayIdx]);
    
    // Vẽ vạch ngăn cách các ô dữ liệu
    canvas->drawFastVLine(xPos - 5, 65, 35, 0x4208); 
  }

  // 4. UI Thông tin trạng thái
  canvas->setTextSize(1);
  canvas->setTextColor(0x7BEF); // Màu xám xanh
  canvas->setCursor(10, 10);
  canvas->print("TURING MACHINE SIMULATION v1.0");
  canvas->setCursor(10, 25);
  canvas->printf("CURRENT STATE: %d", currentState);
  canvas->setCursor(10, 150);
  canvas->print("LOGIC CORE: ESP32-S3 | OPERATOR: TAI");

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
}