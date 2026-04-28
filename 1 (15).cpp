#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCL 12
#define TFT_SDA 11
#define TFT_RES 10
#define TFT_DC   9
#define TFT_CS  13 
#define TFT_BL  21
#define BUTTON_PIN 0 // Nút BOOT

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RES);
GFXcanvas16 *canvas; // Dùng Canvas để tránh xé hình

unsigned long pulseCount = 0;
bool lastButtonState = HIGH;

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);
  pinMode(BUTTON_PIN, INPUT_PULLUP); // Khởi tạo nút BOOT

  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320); // Khởi tạo với độ phân giải màn hình của Tài
  tft.setRotation(1); // Xoay ngang màn hình

  canvas = new GFXcanvas16(320, 170); // Tạo Canvas trong bộ nhớ PSRAM lớn của S3
}

void loop() {
  canvas->fillScreen(ST77XX_BLACK);
  uint16_t gridColor = 0x2104; // Màu xám tối cho lưới nền
  uint16_t waveColor = 0x07FF; // Màu xanh Cyan cho sóng

  // 1. Vẽ lưới nền (Grid) kiểu máy đo dao động
  for(int i=0; i<320; i+=20) canvas->drawFastVLine(i, 0, 170, gridColor);
  for(int i=0; i<170; i+=20) canvas->drawFastHLine(0, i, 320, gridColor);

  // 2. Giả lập và vẽ sóng xung
  static int waveOffset = 0;
  waveOffset = (waveOffset + 4) % 40; // Tốc độ chạy của sóng
  
  int lastY = 120; // Vị trí Y cơ sở của sóng
  for(int x=0; x<320; x++) {
    // Tạo xung vuông: nếu (x + waveOffset) % 40 < 20 thì ở mức cao (High)
    int y = ((x + waveOffset) % 40 < 20) ? 60 : 120; 
    
    // Vẽ đường nối giữa các điểm để tạo xung liên tục
    canvas->drawLine(x-1, lastY, x, y, waveColor);
    lastY = y;
  }

  // 3. Xử lý logic đếm xung & Nút bấm
  bool currentButtonState = digitalRead(BUTTON_PIN);
  if (currentButtonState == LOW && lastButtonState == HIGH) {
    pulseCount = 0; // Reset khi nhấn nút BOOT
  }
  lastButtonState = currentButtonState;
  pulseCount++; // Giả lập đếm xung mỗi khung hình

  // 4. Hiển thị số liệu (Góc UI/UX hiện đại)
  canvas->setTextColor(ST77XX_WHITE);
  canvas->setTextSize(1);
  canvas->setCursor(10, 10);
  canvas->print("PULSE SCOPE | OWNER: TAI");

  canvas->setTextSize(3);
  canvas->setTextColor(ST77XX_YELLOW);
  canvas->setCursor(200, 130); // Đặt ở góc dưới bên phải
  canvas->printf("CT: %lu", pulseCount / 30); // Chia nhỏ để đếm chậm lại dễ nhìn

  // Đẩy toàn bộ hình ảnh từ Canvas ra màn hình (Hardware SPI siêu tốc)
  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
  
  delay(33); // Khống chế khoảng 30 FPS để sóng chạy mượt
}