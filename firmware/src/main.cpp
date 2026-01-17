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

  M5Cardputer.Speaker.begin();
  const auto _global = &GlobalState::getInstance();
  _global->init();
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
  const auto _lcd = &M5Cardputer.Lcd;
  const auto _keeb = &M5Cardputer.Keyboard;

  static bool isLcdSleep = false;
  static unsigned long lastButtonPressed = millis();
  if (_keeb->isPressed()) lastButtonPressed = millis();

  const auto _global = &GlobalState::getInstance();
  const auto screen = _global->getScreen();
  if (screen != nullptr)
  {
    const bool enablePowerSaving = _global->getConfig()->get(
      APP_CONFIG_ENABLE_POWER_SAVING,
      APP_CONFIG_ENABLE_POWER_SAVING_DEFAULT
    ).toInt();
    if (enablePowerSaving && !screen->isKeepScreenOn())
    {
      const auto displayOffInterval = _global->getConfig()->get(
        APP_CONFIG_INTERVAL_DISPLAY_OFF,
        APP_CONFIG_INTERVAL_DISPLAY_OFF_DEFAULT
      ).toInt() * 1000;
      const auto autoPowerOffInterval = _global->getConfig()->get(
        APP_CONFIG_INTERVAL_POWER_OFF,
        APP_CONFIG_INTERVAL_POWER_OFF_DEFAULT
      ).toInt() * 1000 + displayOffInterval;
      if (!isLcdSleep && millis() - lastButtonPressed > displayOffInterval)
      {
        _lcd->sleep();
        _keeb->isChange();
        isLcdSleep = true;
      }

      if (isLcdSleep && millis() - lastButtonPressed <= displayOffInterval)
      {
        _lcd->wakeup();
        _keeb->isChange();
        isLcdSleep = false;
      }

      if (screen->isAutoPowerOff() && isLcdSleep && millis() - lastButtonPressed > autoPowerOffInterval)
      {
        M5Cardputer.Power.powerOff();
      }
    }

    screen->update();
  }
}
