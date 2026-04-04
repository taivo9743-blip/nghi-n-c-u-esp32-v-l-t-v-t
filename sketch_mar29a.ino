#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0);

  tft.fillScreen(TFT_BLACK);

  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(2);

  tft.drawString("ESP32-S3 OK", 30, 100);
}

void loop() {}