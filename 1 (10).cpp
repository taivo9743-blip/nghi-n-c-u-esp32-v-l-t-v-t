#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCL 12
#define TFT_SDA 11
#define TFT_RES 10
#define TFT_DC   9
#define TFT_CS  13 
#define TFT_BL  21
#define ANALOG_PIN 1

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RES);
GFXcanvas16 *canvas;

#define NUM_BARS 20
int barValues[NUM_BARS];
int targetValues[NUM_BARS];

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);

  analogReadResolution(12);
}

void loop() {
  canvas->fillScreen(ST77XX_BLACK);
  
  // 1. Lấy mẫu và xử lý tín hiệu giả lập phổ tần
  int rawInput = analogRead(ANALOG_PIN);
  
  for(int i=0; i<NUM_BARS; i++) {
    // Tạo hiệu ứng nhảy ngẫu nhiên dựa trên input analog để mô phỏng FFT
    targetValues[i] = map(rawInput, 0, 4095, 10, 150) + random(-20, 20);
    if(targetValues[i] < 5) targetValues[i] = 5;
    
    // Làm mượt chuyển động (Easing)
    if(barValues[i] < targetValues[i]) barValues[i] += 15;
    else barValues[i] -= 8;
  }

  // 2. Vẽ giao diện Spectrum
  int barWidth = 320 / NUM_BARS;
  for(int i=0; i<NUM_BARS; i++) {
    uint16_t color;
    // Đổi màu theo độ cao của cột (Xanh -> Vàng -> Đỏ)
    if(barValues[i] < 80) color = 0x07E0;      // Green
    else if(barValues[i] < 120) color = 0xFFE0; // Yellow
    else color = 0xF800;                       // Red

    // Vẽ cột chính
    canvas->fillRect(i * barWidth + 2, 160 - barValues[i], barWidth - 4, barValues[i], color);
    
    // Vẽ vạch đỉnh (Peak) mờ phía trên cho chuyên nghiệp
    canvas->drawFastHLine(i * barWidth + 2, 155 - barValues[i], barWidth - 4, ST77XX_WHITE);
  }

  // 3. UI Header
  canvas->drawFastHLine(0, 162, 320, 0x52AA);
  canvas->setTextSize(1);
  canvas->setTextColor(0x52AA);
  canvas->setCursor(10, 5);
  canvas->print("CORE FREQUENCY ANALYZER | S3 N16R8");
  canvas->setCursor(200, 5);
  canvas->printf("INPUT: GPIO %d", ANALOG_PIN);

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
  delay(10); 
}