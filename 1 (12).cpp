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

void printLog(const char* msg, uint16_t color = ST77XX_GREEN, int wait = 200) {
  tft.setTextColor(color);
  tft.print("[ OK ] ");
  tft.setTextColor(ST77XX_WHITE);
  tft.println(msg);
  delay(wait);
}

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setCursor(0, 0);

  // --- GIAI ĐOẠN 1: BOOT LOG ---
  tft.println("S3-OS KERNEL v2.0.4 - PREPARING BOOT...");
  delay(1000);
  
  printLog("Initializing Xtensa Dual-Core LX7...");
  printLog("Mapping 16MB Flash Memory...");
  printLog("Allocating 8MB Octal PSRAM...");
  printLog("SPI Bus frequency set to 80MHz.");
  printLog("Internal Temperature Sensor: STABLE", 0x07E0);
  printLog("GPIO Matrix configured.");
  
  tft.println("\nLOADING SYSTEM MODULES...");
  for(int i=0; i<=100; i+=10) {
    tft.printf("\rProgress: [%-10s] %d%%", "========== " + (10 - i/10), i); // Cách viết giả lập thanh load
    tft.drawRect(20, 110, 280, 15, 0x52AA);
    tft.fillRect(22, 112, (i * 276) / 100, 11, 0x07E0);
    delay(150);
  }

  delay(500);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  // --- GIAI ĐOẠN 2: TERMINAL INTERFACE ---
  tft.setCursor(0, 0);
  tft.setTextColor(0x07E0); // Màu xanh Terminal cổ điển
  tft.setTextSize(1);
  
  tft.println("******************************************");
  tft.println("* S3 CYBER-INTERFACE - ROOT MODE    *");
  tft.println("* OPERATOR: VO HOANG CONG TAI       *");
  tft.println("******************************************");
  
  tft.setCursor(10, 50);
  tft.setTextColor(ST77XX_WHITE);
  tft.printf("CPU CLK : %d MHz\n", ESP.getCpuFreqMHz());
  tft.setCursor(10, 65);
  tft.printf("FREE HEAP: %d KB\n", ESP.getFreeHeap() / 1024);
  tft.setCursor(10, 80);
  tft.printf("CHIP REV: v%d\n", ESP.getChipRevision());
  
  tft.drawFastHLine(0, 100, 320, 0x52AA);
  
  // Hiệu ứng con trỏ nhấp nháy
  static bool cursorVisible = true;
  tft.fillRect(10, 115, 300, 15, ST77XX_BLACK); // Xóa dòng lệnh
  tft.setCursor(10, 115);
  tft.print("tai@esp32s3:~$ _");
  if(cursorVisible) tft.print(""); else tft.print("\b "); // Giả lập nhấp nháy
  
  // Biểu đồ tài nguyên nhỏ ở góc
  tft.drawRect(200, 120, 100, 40, ST77XX_WHITE);
  tft.setCursor(210, 125); tft.print("CPU LOAD");
  tft.fillRect(210, 140, random(10, 80), 10, 0xF800);

  cursorVisible = !cursorVisible;
  delay(500);
}