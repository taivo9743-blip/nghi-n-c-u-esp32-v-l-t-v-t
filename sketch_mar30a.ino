#include <Adafruit_NeoPixel.h>

#define PIN_NEOPIXEL 48 // Chân điều khiển LED RGB
Adafruit_NeoPixel pixel(1, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800);

const int btnDestruct = 4; // Nút nhấn LA38 (Màu đỏ to)
const int btnLock = 5;     // Nút gạt CRSL

// Biến trạng thái
bool isDestructTriggered = false;
bool lastLockState = HIGH;
unsigned long buttonPressStartTime = 0;
unsigned long lastFlashTime = 0;
bool ledState = false;

// Tọa độ LAT/LON giả lập
float lat = 10.7626;
float lon = 106.6602;

void setup() {
  pixel.begin();
  pixel.setBrightness(50);
  Serial.begin(115200);
  
  pinMode(btnDestruct, INPUT_PULLUP);
  pinMode(btnLock, INPUT_PULLUP);
  
  // Trạng thái ban đầu: Bình thường (Xanh lá)
  setPixel(0, 255, 0); 
}

void loop() {
  // --- 1. NHẬN LỆNH TỪ PYTHON (ĐỒNG BỘ WEB) ---
  if (Serial.available() > 0) {
    String webCmd = Serial.readStringUntil('\n');
    webCmd.trim();

    if (webCmd == "L_YELLOW") {
      isDestructTriggered = false;
      setPixel(255, 200, 0); // Vàng: Hệ thống đã sẵn sàng
    } 
    else if (webCmd == "L_RED") {
      isDestructTriggered = true; // Đỏ: Bắt đầu nháy báo động
    } 
    else if (webCmd == "L_OFF") {
      isDestructTriggered = false;
      setPixel(0, 255, 0); // Xanh: Chế độ an toàn
    }
  }

  // --- 2. LOGIC NÚT BẤM VẬT LÝ ---
  bool currentLockState = digitalRead(btnLock);
  bool currentDestructBtn = digitalRead(btnDestruct);

  // Xử lý nút Gạt (CRSL) - Ưu tiên cao nhất
  if (currentLockState != lastLockState) {
    delay(50); // Chống rung (Debounce)
    if (currentLockState == LOW) { // Gạt sang ON
      Serial.println("SYSTEM_ACTIVATED"); 
      if (!isDestructTriggered) setPixel(255, 200, 0); 
    } else { // Gạt về OFF
      Serial.println("SYSTEM_LOCKED"); // Gửi lệnh để Python xóa khung vàng
      isDestructTriggered = false;
      setPixel(0, 255, 0); 
    }
    lastLockState = currentLockState;
  }

  // Xử lý nút nhấn Tự hủy (Chỉ khi nút Gạt đang ON)
  if (currentLockState == LOW) {
    if (currentDestructBtn == LOW) {
      if (buttonPressStartTime == 0) buttonPressStartTime = millis();
      
      // Nhấn giữ đủ 1.5 giây để kích hoạt
      if (millis() - buttonPressStartTime >= 1500 && !isDestructTriggered) {
        // Gửi thông tin tọa độ giả lập để Python "vẽ" lại màn hình
        Serial.print("ACTIVATE_SELF_DESTRUCT: Lat=");
        Serial.print(lat, 4);
        Serial.print(", Lon=");
        Serial.println(lon, 4);
        isDestructTriggered = true;
      }
    } else {
      buttonPressStartTime = 0;
    }
  }

  // --- 3. HIỆU ỨNG NHÁY ĐỎ ---
  if (isDestructTriggered) {
    if (millis() - lastFlashTime > 150) { 
      ledState = !ledState;
      if (ledState) setPixel(255, 0, 0); // Đỏ rực
      else setPixel(0, 0, 0);           // Tắt đen
      lastFlashTime = millis();
    }
  }
}

// Hàm phụ để set màu nhanh
void setPixel(int r, int g, int b) {
  pixel.setPixelColor(0, pixel.Color(r, g, b));
  pixel.show();
}