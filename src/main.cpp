#include <Arduino.h>
#include <M5Cardputer.h>
#include <SD.h>

#include "os/ScreenManager.hpp"
#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/HelloScreen.hpp"

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

ScreenManager screenManager;

void setup()
{
  M5Cardputer.begin();

  Serial.begin(115200);
  M5Cardputer.Lcd.setTextColor(BLUE, BLACK);
  M5Cardputer.Lcd.setTextSize(2);
  M5Cardputer.Lcd.drawCenterString(
    "Booting",
    M5Cardputer.Lcd.width() / 2,
    M5Cardputer.Lcd.height() / 2 - M5Cardputer.Lcd.fontHeight() / 2
  );

  // SD Card Initialization
  Serial.println("load SDCard");
  SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

  if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000))
  {
    Serial.println("Not loaded SDCard");
    M5Cardputer.Lcd.drawCenterString(
      "SDCard is not loaded",
      M5Cardputer.Lcd.width() / 2,
      M5Cardputer.Lcd.height() / 2 - M5Cardputer.Lcd.fontHeight() / 2
    );

    while (1);
  }

  Serial.println("Start screen manager");
  M5Cardputer.Lcd.drawCenterString(
    "Loaded",
    M5Cardputer.Lcd.width() / 2,
    M5Cardputer.Lcd.height() / 2 - M5Cardputer.Lcd.fontHeight() / 2
  );

  Serial.println("start main");
  screenManager.setScreen(new FileNavigatorScreen("/"));
}

void loop()
{
  M5Cardputer.update();
  screenManager.update();
}
