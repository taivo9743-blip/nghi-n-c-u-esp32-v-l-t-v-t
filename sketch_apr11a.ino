#include <Arduino_GFX_Library.h>
#include <DHT.h>
#include <Adafruit_NeoPixel.h>

// ===== TFT PIN =====
#define TFT_SCK  12
#define TFT_MOSI 11
#define TFT_MISO -1
#define TFT_CS   13
#define TFT_DC   9
#define TFT_RST  10
#define TFT_BL   14

// ===== LED =====
#define LED_PIN 48
#define NUMPIXELS 1
Adafruit_NeoPixel pixel(NUMPIXELS, LED_PIN, NEO_GRB + NEO_KHZ800);

// ===== DHT =====
#define DHTPIN   4
#define DHTTYPE  DHT11
DHT dht(DHTPIN, DHTTYPE);

// ===== COLOR =====
#define BLACK  0x0000
#define WHITE  0xFFFF
#define GREEN  0x07E0
#define RED    0xF800
#define YELLOW 0xFFE0
#define CYAN   0x07FF
#define GRAY   0x8410
#define BLUE   0x001F

Arduino_DataBus *bus = new Arduino_ESP32SPI(
  TFT_DC, TFT_CS, TFT_SCK, TFT_MOSI, TFT_MISO
);

Arduino_GFX *gfx = new Arduino_ST7789(
  bus, TFT_RST,
  0,
  true,
  170, 320,
  35, 0
);

// ===== STATE =====
float oldT = -100, oldH = -100;

// ===== TEMP COLOR =====
uint16_t getTempColor(float t)
{
  if (t < 30) return BLUE;
  else if (t < 37) return GREEN;
  else if (t < 42) return YELLOW;
  else return RED;
}

// ===== LED =====
void setLED(float t)
{
  uint32_t color;

  if (t < 30) color = pixel.Color(0, 0, 255);
  else if (t < 37) color = pixel.Color(0, 255, 0);
  else if (t < 42) color = pixel.Color(255, 150, 0);
  else color = pixel.Color(255, 0, 0);

  pixel.setPixelColor(0, color);
  pixel.show();
}

void dangerBlink()
{
  pixel.setPixelColor(0, pixel.Color(255, 0, 0));
  pixel.show();
  delay(80);

  pixel.clear();
  pixel.show();
  delay(80);
}

// ===== BAR =====
void drawBar(int x, int y, int w, int h, int value, int max, uint16_t color)
{
  int fill = map(value, 0, max, 0, w);
  if (fill < 0) fill = 0;

  gfx->drawRoundRect(x, y, w, h, 4, GRAY);
  gfx->fillRoundRect(x + 2, y + 2, fill - 4, h - 4, 3, color);
}

// ===== CENTER TEXT =====
void drawCentered(int x, int y, int w, String text)
{
  int16_t x1, y1;
  uint16_t w1, h1;
  gfx->getTextBounds(text, 0, 0, &x1, &y1, &w1, &h1);

  int cx = x + (w - w1) / 2;
  gfx->setCursor(cx, y);
  gfx->print(text);
}

// ===== UI =====
void drawUI()
{
  gfx->fillScreen(BLACK);

  gfx->setTextColor(CYAN);
  gfx->setTextSize(2);
  drawCentered(0, 5, 320, "NHIET DO PHONG TAO ");

  gfx->drawRoundRect(10, 30, 140, 130, 10, GRAY);
  gfx->drawRoundRect(170, 30, 140, 130, 10, GRAY);

  gfx->setTextSize(2);

  gfx->setTextColor(YELLOW);
  gfx->setCursor(35, 35);
  gfx->print("NHIET DO");

  gfx->setTextColor(GREEN);
  gfx->setCursor(200, 35);
  gfx->print("DO AM");
}

void setup()
{
  Serial.begin(115200);

  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  gfx->begin();
  gfx->setRotation(3);

  pixel.begin();
  pixel.clear();
  pixel.show();

  drawUI();
  dht.begin();
}

void loop()
{
  float t = dht.readTemperature();
  float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) return;

  setLED(t);

  uint16_t color = getTempColor(t);

  // ===== TEMP =====
  if (abs(t - oldT) > 0.2)
  {
    oldT = t;

    gfx->fillRect(15, 60, 130, 60, BLACK);

    gfx->setTextColor(color);
    gfx->setTextSize(4);
    drawCentered(10, 60, 140, String(t, 1) + "C");

    drawBar(20, 110, 120, 12, t, 60, color);
  }

  // ===== HUM =====
  if (abs(h - oldH) > 1)
  {
    oldH = h;

    gfx->fillRect(175, 60, 130, 60, BLACK);

    gfx->setTextColor(CYAN);
    gfx->setTextSize(4);
    drawCentered(170, 60, 140, String(h, 1) + "%");

    drawBar(180, 110, 120, 12, h, 100, BLUE);
  }

  // ===== DANGER =====
  if (t >= 42)
  {
    gfx->drawRoundRect(10, 30, 140, 130, 10, RED);
    dangerBlink();
    gfx->drawRoundRect(10, 30, 140, 130, 10, GRAY);
  }

  // ===== SERIAL =====
  Serial.print("T:");
  Serial.print(t, 1);
  Serial.print(",H:");
  Serial.println(h, 1);

  delay(800);
}