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

void printLog(const char* msg, uint16_t color = ST77XX_GREEN, int wait = 150) {
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

  // --- BOOT SEQUENCE ---
  tft.println("S3-OS KERNEL v4.1.0 - INITIALIZING...");
  delay(800);
  
  printLog("Detecting Xtensa Dual-Core LX7...");
  printLog("Mounting 16MB Flash Storage...");
  printLog("Mapping 8MB Octal PSRAM...");
  printLog("Loading SPI Bus @ 80MHz...");
  printLog("DMA Channel 0: READY");
  printLog("Internal Thermal Sensor: STABLE");
  
  tft.println("\nLOADING SYSTEM UI...");
  for(int i=0; i<=100; i+=20) {
    tft.printf("\rStatus: [%-10s] %d%%", "##########", i);
    tft.drawRect(20, 110, 280, 15, 0x52AA);
    tft.fillRect(22, 112, (i * 276) / 100, 11, 0x07E0);
    delay(200);
  }

  delay(500);
  tft.fillScreen(ST77XX_BLACK);
}

void loop() {
  tft.setCursor(0, 0);
  tft.setTextColor(0x07E0); // Màu xanh lá Terminal cổ điển
  
  tft.println("******************************************");
  tft.println("* S3 CYBER-INTERFACE - ROOT ACCESS      *");
  tft.println("* OPERATOR: VO HOANG CONG TAI           *");
  tft.println("******************************************");
  
  tft.setCursor(10, 50);
  tft.setTextColor(ST77XX_WHITE);
  tft.printf("CPU CLOCK : %d MHz\n", ESP.getCpuFreqMHz());
  tft.setCursor(10, 65);
  tft.printf("FREE HEAP : %d KB\n", ESP.getFreeHeap() / 1024);
  tft.setCursor(10, 80);
  tft.printf("CHIP REV  : v%d\n", ESP.getChipRevision());
  
  tft.drawFastHLine(0, 100, 320, 0x52AA);
  
  // Hiệu ứng con trỏ nhấp nháy
  static bool cursorVisible = true;
  tft.fillRect(10, 115, 300, 15, ST77XX_BLACK); // Xóa dòng cũ để ghi đè
  tft.setCursor(10, 115);
  tft.print("tai@esp32s3:~$ _");
  if(cursorVisible) tft.print(""); else tft.print("\b "); 
  
  // Biểu đồ nhỏ giả lập CPU load
  tft.drawRect(200, 120, 100, 40, ST77XX_WHITE);
  tft.setCursor(210, 125); tft.print("SYS_LOAD");
  tft.fillRect(210, 140, random(10, 85), 10, 0xF800);

  cursorVisible = !cursorVisible;
  delay(500);
}