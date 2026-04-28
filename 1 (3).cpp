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

int rotors[3] = {0, 0, 0};
const char* originalText = "VO HOANG CONG TAI";
char encryptedText[20];

void encrypt() {
  for(int i=0; i < strlen(originalText); i++) {
    char c = originalText[i];
    if(c >= 'A' && c <= 'Z') {
      // Logic mã hóa qua 3 tầng bánh răng
      c = ((c - 'A' + rotors[0]) % 26) + 'A';
      c = ((c - 'A' + rotors[1]) % 26) + 'A';
      c = ((c - 'A' + rotors[2]) % 26) + 'A';
    }
    encryptedText[i] = c;
  }
  encryptedText[strlen(originalText)] = '\0';
}

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);
  encrypt();
}

void loop() {
  canvas->fillScreen(ST77XX_BLACK);
  uint16_t gold = 0xFEA0;

  // 1. Vẽ 3 bánh răng mã hóa (Rotors)
  for(int i=0; i<3; i++) {
    int cx = 60 + (i * 100);
    int cy = 85;
    canvas->drawCircle(cx, cy, 40, 0x4208);
    
    // Vẽ các vạch răng cưa quay
    for(int a=0; a<360; a+=30) {
      float rad = (a + rotors[i] * 10) * PI / 180.0;
      canvas->drawLine(cx + cos(rad)*30, cy + sin(rad)*30, cx + cos(rad)*40, cy + sin(rad)*40, gold);
    }
    rotors[i] = (rotors[i] + (i+1)) % 360; // Tốc độ quay khác nhau
  }

  // 2. Hiển thị dữ liệu mã hóa
  canvas->setTextSize(2);
  canvas->setTextColor(ST77XX_WHITE);
  canvas->setCursor(10, 10);
  canvas->print("PLAIN:  "); canvas->print(originalText);
  
  canvas->setTextColor(0x07E0);
  canvas->setCursor(10, 140);
  canvas->print("CIPHER: "); canvas->print(encryptedText);

  // 3. Tương tác nút bấm
  if(digitalRead(BUTTON_PIN) == LOW) {
    rotors[0] += random(10, 50); // Đổi khóa ngẫu nhiên
    encrypt();
  }

  canvas->setTextSize(1);
  canvas->setTextColor(0x52AA);
  canvas->setCursor(100, 160);
  canvas->print("ENIGMA S3 LOGIC | OP: TAI");

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
  delay(50);
}