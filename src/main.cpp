#include <Arduino.h>
#include <M5Cardputer.h>
#include <SD.h>

#include "os/core/ScreenManager.hpp"
#include "os/screens/HelloScreen.hpp"

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

Lua lua;

void setup()
{
  M5Cardputer.begin();

  Serial.begin(115200);
  M5Cardputer.Lcd.setTextColor(BLUE, BLACK);
  M5Cardputer.Lcd.setTextSize(2.5);
  M5Cardputer.Lcd.drawCenterString(
    "PuterOS",
    M5Cardputer.Lcd.width() / 2,
    M5Cardputer.Lcd.height() / 2 - M5Cardputer.Lcd.fontHeight() / 2
  );

  // SD Card Initialization
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

  if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000))
  {
    M5Cardputer.Lcd.drawCenterString(
      "SDCard is not loaded",
      M5Cardputer.Lcd.width() / 2,
      M5Cardputer.Lcd.height() / 2 - M5Cardputer.Lcd.fontHeight() / 2
    );

    while (1);
  }
;
  ScreenManager::getInstance().setScreen(new HelloScreen());
}

void loop()
{
  M5Cardputer.update();
  ScreenManager::getInstance().update();
}
