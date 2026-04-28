#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCL 12
#define TFT_SDA 11
#define TFT_RES 10
#define TFT_DC   9
#define TFT_CS  13 
#define TFT_BL  21
#define ANALOG_PIN 1 // Chân đọc tín hiệu (Anten)

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RES);
GFXcanvas16 *canvas;

int samples[320]; // Lưu trữ mẫu tín hiệu để vẽ

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);

  analogReadResolution(12); // Độ phân giải 12-bit (0-4095)
}

void loop() {
  canvas->fillScreen(ST77XX_BLACK);
  
  // 1. Đọc mẫu tín hiệu cực nhanh
  for(int i=0; i<320; i++) {
    samples[i] = analogRead(ANALOG_PIN);
    delayMicroseconds(50); // Tốc độ lấy mẫu
  }

  // 2. Vẽ lưới điện tử màu tím mờ
  for(int i=0; i<320; i+=40) canvas->drawFastVLine(i, 0, 170, 0x2004);
  for(int i=0; i<170; i+=40) canvas->drawFastHLine(0, i, 320, 0x2004);

  // 3. Vẽ dạng sóng (Waveform)
  for(int i=1; i<320; i++) {
    // Ánh xạ giá trị Analog (0-4095) vào chiều cao màn hình (0-170)
    int y1 = map(samples[i-1], 0, 4095, 160, 10);
    int y2 = map(samples[i], 0, 4095, 160, 10);
    
    // Hiệu ứng đổ bóng mờ (Glow)
    canvas->drawLine(i-1, y1+1, i, y2+1, 0x03E0); // Màu xanh lá mờ
    canvas->drawLine(i-1, y1, i, y2, ST77XX_GREEN); // Đường sóng chính
  }

  // 4. UI Thông số
  canvas->setTextColor(0x07E0);
  canvas->setTextSize(1);
  canvas->setCursor(10, 10);
  canvas->print("SIGNAL SCANNER: ACTIVE");
  canvas->setCursor(200, 10);
  canvas->printf("SAMPLES: 320pt");
  canvas->setCursor(10, 155);
  canvas->print("OPERATOR: VO HOANG CONG TAI");

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
}