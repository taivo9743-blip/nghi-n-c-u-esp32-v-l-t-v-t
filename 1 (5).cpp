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
uint32_t currentAddress = 0x3FCA0000; // Một vùng địa chỉ RAM mặc định của S3

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);
}

void drawMemoryDump(uint32_t startAddr) {
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  
  // Header
  tft.setTextColor(0xF800); // Màu đỏ cảnh báo
  tft.setCursor(10, 5);
  tft.print("S3 LIVE MEMORY INSPECTOR - UNRESTRICTED ACCESS");
  tft.drawFastHLine(0, 15, 320, 0x4208);

  tft.setTextColor(0x07E0); // Màu xanh Terminal
  int lines = 14;
  int bytesPerLine = 8;

  for (int i = 0; i < lines; i++) {
    uint32_t addr = startAddr + (i * bytesPerLine);
    tft.setCursor(5, 25 + (i * 10));
    
    // In địa chỉ ô nhớ
    tft.setTextColor(0xFDA0); // Màu Amber
    tft.printf("%08X: ", addr);
    
    // In dữ liệu Hex
    tft.setTextColor(ST77XX_WHITE);
    char ascii[bytesPerLine + 1];
    for (int j = 0; j < bytesPerLine; j++) {
      uint8_t val = *(uint8_t*)(addr + j); // Đọc trực tiếp từ ô nhớ
      tft.printf("%02X ", val);
      
      // Lưu lại ký tự ASCII nếu đọc được
      if (val >= 32 && val <= 126) ascii[j] = (char)val;
      else ascii[j] = '.';
    }
    ascii[bytesPerLine] = '\0';

    // In cột ASCII bên phải
    tft.setTextColor(0x07FF); // Màu Cyan
    tft.printf("| %s", ascii);
  }

  // Footer
  tft.drawFastHLine(0, 160, 320, 0x4208);
  tft.setCursor(10, 162);
  tft.setTextColor(0x7BEF);
  tft.printf("REGION: RAM | ADDR_PTR: 0x%08X | OP: TAI", startAddr);
}

void loop() {
  static bool firstRun = true;
  if (firstRun || digitalRead(BUTTON_PIN) == LOW) {
    if(!firstRun) currentAddress += 0x100; // Nhảy vùng nhớ mỗi lần nhấn nút
    drawMemoryDump(currentAddress);
    firstRun = false;
    delay(300); // Chống dội nút
  }
}