#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>

#define TFT_SCL 12
#define TFT_SDA 11
#define TFT_RES 10
#define TFT_DC   9
#define TFT_CS  13 
#define TFT_BL  21

// Độ phân giải của thế giới Game of Life (Tối ưu cho S3)
#define GRID_WIDTH  160 
#define GRID_HEIGHT 85  

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RES);
GFXcanvas16 *canvas;

// Hai lưới để lưu trạng thái hiện tại và thế hệ tiếp theo
uint8_t *grid[2]; 
int currentGrid = 0;
int generation = 0;

void setup() {
  pinMode(TFT_BL, OUTPUT);
  digitalWrite(TFT_BL, HIGH);

  SPI.begin(TFT_SCL, -1, TFT_SDA, TFT_CS); 
  tft.init(170, 320);
  tft.setRotation(1);
  canvas = new GFXcanvas16(320, 170);

  // Cấp phát bộ nhớ cho hai lưới trong PSRAM
  grid[0] = (uint8_t *)ps_malloc(GRID_WIDTH * GRID_HEIGHT);
  grid[1] = (uint8_t *)ps_malloc(GRID_WIDTH * GRID_HEIGHT);

  // Khởi tạo thế hệ ban đầu ngẫu nhiên
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextSize(1);
  tft.setTextColor(ST77XX_WHITE);
  tft.setCursor(10, 10);
  tft.println("INITIALIZING WORLD...");
  
  randomSeed(analogRead(0));
  for (int i = 0; i < GRID_WIDTH * GRID_HEIGHT; i++) {
    grid[0][i] = random(2); 
  }
  delay(1000);
}

// Hàm đếm số láng giềng sống của một tế bào
int countAliveNeighbors(int x, int y, int gridIdx) {
  int count = 0;
  for (int i = -1; i <= 1; i++) {
    for (int j = -1; j <= 1; j++) {
      if (i == 0 && j == 0) continue; 
      int nx = (x + i + GRID_WIDTH) % GRID_WIDTH;
      int ny = (y + j + GRID_HEIGHT) % GRID_HEIGHT;
      if (grid[gridIdx][ny * GRID_WIDTH + nx] == 1) {
        count++;
      }
    }
  }
  return count;
}

void loop() {
  canvas->fillScreen(ST77XX_BLACK);
  uint16_t color = tft.color565(0, random(100,255), random(100,255)); // Màu sinh vật đổi ngẫu nhiên

  int nextGrid = 1 - currentGrid;

  // 1. Tính toán thế hệ tiếp theo
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      int idx = y * GRID_WIDTH + x;
      int aliveNeighbors = countAliveNeighbors(x, y, currentGrid);

      if (grid[currentGrid][idx] == 1) {
        // Quy luật cho tế bào sống
        if (aliveNeighbors < 2 || aliveNeighbors > 3) {
          grid[nextGrid][idx] = 0; // Chết (quá ít hoặc quá nhiều láng giềng)
        } else {
          grid[nextGrid][idx] = 1; // Sống (2-3 láng giềng)
        }
      } else {
        // Quy luật cho tế bào chết
        if (aliveNeighbors == 3) {
          grid[nextGrid][idx] = 1; // Sống lại (đúng 3 láng giềng)
        } else {
          grid[nextGrid][idx] = 0; // Vẫn chết
        }
      }
    }
  }

  // 2. Vẽ thế hệ hiện tại lên Canvas
  for (int y = 0; y < GRID_HEIGHT; y++) {
    for (int x = 0; x < GRID_WIDTH; x++) {
      if (grid[nextGrid][y * GRID_WIDTH + x] == 1) {
        // Vẽ tế bào sống dưới dạng ô vuông 2x2
        canvas->fillRect(x * 2, y * 2, 2, 2, color);
      }
    }
  }

  // Chuyển sang thế hệ tiếp theo
  currentGrid = nextGrid;
  generation++;

  // UI Thông số
  canvas->setTextSize(1);
  canvas->setTextColor(ST77XX_WHITE);
  canvas->setCursor(10, 155);
  canvas->printf("GEN: %d | WORLD: %dx%d | OWNER: TAI", generation, GRID_WIDTH, GRID_HEIGHT);

  // Đẩy từ Canvas ra màn hình
  tft.drawRGBBitmap(0, 0, canvas->getBuffer(), 320, 170);
}