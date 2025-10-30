#include <Arduino.h>
#include <M5Cardputer.h>

#include "os/GlobalState.hpp"
#include "os/screens/WelcomeScreen.hpp"

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

  const auto _global = &GlobalState::getInstance();
  _global->loadSDCard();
  _global->setScreen(new WelcomeScreen());
  const auto config = _global->getConfig();
  const auto brightness = config->get("brightness", APP_CONFIG_BRIGHTNESS_DEFAULT).toInt();
  M5Cardputer.Lcd.setBrightness(static_cast<uint8_t>(brightness / 100.0 * 255));

  const auto volume = config->get(APP_CONFIG_VOLUME, APP_CONFIG_VOLUME_DEFAULT).toInt();
  M5Cardputer.Speaker.setVolume(static_cast<uint8_t>(volume / 100.0 * 255));
}

void loop()
{
  M5Cardputer.update();

  const auto _global = &GlobalState::getInstance();
  const auto screen = _global->getScreen();
  if (screen != nullptr) screen->update();
}
