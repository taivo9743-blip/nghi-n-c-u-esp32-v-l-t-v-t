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

#define NUM_STARS 150
struct Star {
  float x, y, z;
  float oldZ;
};

Star stars[NUM_STARS];
float speed = 2.0;

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);

  // Khởi tạo vị trí sao ngẫu nhiên trong không gian 3D
  for(int i=0; i<NUM_STARS; i++) {
    stars[i].x = random(-500, 500);
    stars[i].y = random(-500, 500);
    stars[i].z = random(100, 1000);
    stars[i].oldZ = stars[i].z;
  }
}

void loop() {
  canvas->fillScreen(ST77XX_BLACK);

  // Nhấn nút BOOT để tăng tốc độ (Warp Drive)
  if (digitalRead(BUTTON_PIN) == LOW) {
    speed = 25.0; // Tốc độ ánh sáng
  } else {
    speed = 4.0;  // Tốc độ bình thường
  }

  for(int i=0; i<NUM_STARS; i++) {
    stars[i].oldZ = stars[i].z;
    stars[i].z -= speed;

    // Reset sao khi nó bay qua mặt người nhìn
    if(stars[i].z <= 1) {
      stars[i].z = 1000;
      stars[i].x = random(-500, 500);
      stars[i].y = random(-500, 500);
      stars[i].oldZ = stars[i].z;
    }

    // Chiếu tọa độ 3D xuống 2D (Perspective Projection)
    // Công thức: x_screen = x_3d / z_3d
    float x1 = (stars[i].x / stars[i].z) * 100 + 160;
    float y1 = (stars[i].y / stars[i].z) * 100 + 85;
    
    float x2 = (stars[i].x / stars[i].oldZ) * 100 + 160;
    float y2 = (stars[i].y / stars[i].oldZ) * 100 + 85;

    // Vẽ tia sáng (streak)
    uint16_t color = tft.color565(255 - (stars[i].z/4), 255 - (stars[i].z/4), 255);
    canvas->drawLine(x1, y1, x2, y2, color);
  }

  // UI HUD trang trí
  canvas->drawRect(5, 5, 310, 160, 0x0841);
  canvas->setTextColor(0x07E0);
  canvas->setCursor(10, 15);
  canvas->print("WARP DRIVE: ");
  canvas->print(speed > 10 ? "ACTIVE" : "READY");
  canvas->setCursor(200, 15);
  canvas->print("NAV: V.H.C. TAI");

  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
}