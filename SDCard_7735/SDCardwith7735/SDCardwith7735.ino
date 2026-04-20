/* HARDWARE PIN SETUP (ESP32)
 * -------------------------------------------
 * ST7735 TFT SCREEN:
 * - VCC    -> 3.3V
 * - GND    -> GND
 * - SCL    -> GPIO 18 (Shared SCLK)
 * - SDA    -> GPIO 23 (Shared MOSI)
 * - RES    -> GPIO 4  (Reset)
 * - DC/A0  -> GPIO 2  (Data/Command)
 * - CS     -> GPIO 17 (TFT Chip Select)
 * - BLK    -> 3.3V    (Backlight)
 *
 * SD CARD MODULE:
 * - VCC    -> 3.3V
 * - GND    -> GND
 * - MISO   -> GPIO 19 (Master In Slave Out)
 * - MOSI   -> GPIO 23 (Shared with TFT SDA)
 * - SCLK   -> GPIO 18 (Shared with TFT SCL)
 * - CS     -> GPIO 5  (SD Chip Select)
 */


#include <SPI.h>
#include <SD.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

// Pin Definitions
#define SD_CS     5
#define TFT_CS   17
#define TFT_DC    2 
#define TFT_RST   4

Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// Routine to draw each pixels using the buffer form file Picture.raw
// Each pixels is 2 bytes
void drawRaw(const char* filename, int16_t x, int16_t y, int16_t w, int16_t h) {
  File imgFile = SD.open(filename);

  if (!imgFile) {
    Serial.println("File not found!");
    return;
  }

  // Set drawing window
  tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

  uint8_t buffer[64];

  while (imgFile.available()) {
    int bytesRead = imgFile.read(buffer, sizeof(buffer));

    for (int i = 0; i < bytesRead; i += 2) {
      uint16_t color = (buffer[i+1] << 8) | buffer[i];
      tft.pushColor(color);
    }
  }

  imgFile.close();
}

void setup() {
  delay(1500);
  Serial.begin(115200);

  // Set pins High off mode
  pinMode(TFT_CS, OUTPUT);
  pinMode(SD_CS, OUTPUT);
  digitalWrite(TFT_CS, HIGH);
  digitalWrite(SD_CS, HIGH);

  tft.initR(INITR_BLACKTAB);

  // ✅ Portrait mode (128x160) (Raw wont work in Landscape)
  tft.setRotation(0);

  tft.fillScreen(ST7735_BLACK);

  if (!SD.begin(SD_CS)) {
    tft.println("SD Fail!");
    return;
  }

  // ✅ IMPORTANT: width = 128, height = 160
  drawRaw("/Family.raw", 0, 0, 128, 160);
}

void loop() {}