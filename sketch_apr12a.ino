#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <DHT.h>

#define TFT_CS 37
#define TFT_DC 38
#define TFT_RST 39
#define TFT_MOSI 35
#define TFT_SCLK 36
#define DHTPIN 4

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
DHT dht(DHTPIN, DHT11);

String inputString = "";
String currentTime = "00:00:00";
String currentDate = "01/01/2026";
float lastT = 0, lastH = 0;

void setup() {
  Serial.begin(115200);
  SPI.begin(TFT_SCLK, -1, TFT_MOSI, TFT_CS);
  tft.init(170, 320);
  tft.setRotation(1);
  tft.fillScreen(ST77XX_BLACK);
  dht.begin();
  
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  tft.setCursor(20, 20);
  tft.print("Waiting for Python...");
}

void loop() {
  // 1. Doc du lieu tu Serial
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      if (inputString.startsWith("TIME:")) {
        // Tach chuoi: TIME:HH:MM:SS DD/MM/YYYY
        currentTime = inputString.substring(5, 13);
        currentDate = inputString.substring(14);
        updateDisplay();
      }
      inputString = "";
    } else {
      inputString += inChar;
    }
  }

  // 2. Doc cam bien (moi 2 giay)
  static unsigned long lastDHT = 0;
  if (millis() - lastDHT > 2000) {
    float t = dht.readTemperature();
    float h = dht.readHumidity();
    if (!isnan(t) && !isnan(h)) {
      drawSensors(t, h);
    }
    lastDHT = millis();
  }
}

void updateDisplay() {
  // Ve gio (Su dung mau nen de xoa chu cu - chong nhay)
  tft.setTextColor(ST77XX_CYAN, ST77XX_BLACK);
  tft.setTextSize(6);
  tft.setCursor(20, 25);
  tft.print(currentTime.substring(0, 5)); // HH:MM

  tft.setTextSize(3);
  tft.setCursor(210, 50);
  tft.print(currentTime.substring(5)); // :SS

  tft.setTextSize(2);
  tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft.setCursor(20, 75);
  tft.print(currentDate);
}

void drawSensors(float t, float h) {
  tft.drawFastHLine(0, 105, 320, 0x4208);
  
  tft.setTextSize(3);
  tft.setTextColor(ST77XX_ORANGE, ST77XX_BLACK);
  tft.setCursor(20, 120);
  tft.printf("%.1f C", t);

  tft.setTextColor(0x5DFF, ST77XX_BLACK);
  tft.setCursor(185, 120);
  tft.printf("%.0f %%", h);
}