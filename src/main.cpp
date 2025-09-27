#include <Arduino.h>
#include <M5Cardputer.h>
#include <SD.h>

#include "os/GlobalState.hpp"
#include "os/screens/WelcomeScreen.hpp"

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

void setup()
{
  M5Cardputer.begin();

  Serial.begin(115200);
  M5Cardputer.Lcd.setTextColor(DARKCYAN, BLACK);
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

    while (true);
  }

  const auto _global = &GlobalState::getInstance();
  _global->setScreen(new WelcomeScreen());
  const auto config = _global->getConfig();
  const auto brightness = config->get("brightness", "90").toInt();
  M5Cardputer.Lcd.setBrightness(brightness / 100.0 * 255);
}

void loop()
{
  M5Cardputer.update();

  const auto _global = &GlobalState::getInstance();
  const auto screen = _global->getScreen();
  if (screen != nullptr) screen->update();
}
