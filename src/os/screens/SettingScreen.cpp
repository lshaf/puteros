//
// Created by l-sha on 9/23/2025.
//

#include "os/screens/SettingScreen.h"

#include "lgfx/utility/lgfx_qrcode.h"
#include "os/component/InputNumberScreen.h"
#include "os/component/InputTextScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"
#include "os/utility/AudioUtility.h"
#include "os/utility/HelperUtility.h"
#include "os/utility/SoundNotification.h"

void SettingScreen::init()
{
  refreshMenu(true);
}

void SettingScreen::refreshMenu(const bool reset)
{
  currentState = STATE_MAIN;
  const String brightnessValue = _config->get(APP_CONFIG_BRIGHTNESS, APP_CONFIG_BRIGHTNESS_DEFAULT);
  const String name = _config->get(APP_CONFIG_DEVICE_NAME, APP_CONFIG_DEVICE_NAME_DEFAULT);
  const String volume = _config->get(APP_CONFIG_VOLUME, APP_CONFIG_VOLUME_DEFAULT);
  const bool navSound = _config->get(APP_CONFIG_NAV_SOUND, APP_CONFIG_NAV_SOUND_DEFAULT).toInt();
  Template::renderHead("Settings");
  std::vector<ListEntryItem> finalEntries = {
    {"Name", name.c_str()},
    {"Brightness", brightnessValue.c_str()},
    {"Volume", volume.c_str()},
    {"Navigation Sound", navSound ? "On" : "Off"},
    {"About", ""}
  };

  if (reset) setEntries(finalEntries);
  else
  {
    entries = finalEntries;
    render();
  }
}

void SettingScreen::renderAbout()
{
  currentState = STATE_ABOUT;
  Template::renderHead("About", false);

  auto body = Template::createBody();
  body.fillSprite(BLACK);
  body.setTextColor(TFT_WHITE);
  body.setTextSize(1.5);
  body.drawCenterString("PuterOS v1.0.0", body.width() / 2, body.height() / 2 - body.fontHeight());
  body.drawCenterString("Powered by M5Stack", body.width() / 2, body.height() / 2);
  Template::renderBody(&body);
}

void SettingScreen::onEnter(const ListEntryItem entry)
{
  if (entry.label == "Brightness")
  {
    const int currentValue = std::stoi(entry.value);
    const int newBrightness = InputNumberScreen::popup("Brightness", currentValue, 5, 100);
    if (newBrightness != currentValue)
    {
      _config->set(APP_CONFIG_BRIGHTNESS, String(newBrightness));
      const bool saved = _config->save();
      if (saved) M5Cardputer.Lcd.setBrightness(static_cast<uint8_t>(newBrightness / 100.0 * 255));
      else
      {
        Template::renderStatus("Failed to save brightness.", TFT_RED);
        HelperUtility::delayMs(1500);
      }
    }

    refreshMenu();
  } else if (entry.label == "Volume")
  {
    const int currentValue = std::stoi(entry.value);
    const int newVolume = InputNumberScreen::popup("Volume", currentValue, 0, 100);
    if (newVolume != currentValue)
    {
      _config->set(APP_CONFIG_VOLUME, String(newVolume));
      const bool saved = _config->save();
      if (saved)
      {
        M5Cardputer.Speaker.setVolume(static_cast<uint8_t>(newVolume / 100.0 * 255));
        AudioUtility::playNotification();
      } else
      {
        Template::renderStatus("Failed to save volume.", TFT_RED);
        HelperUtility::delayMs(1500);
      }
    }

    refreshMenu();
  } else if (entry.label == "Name")
  {
    const auto newName = InputTextScreen::popup("Device Name", entry.value);
    if (!newName.empty() && newName.size() <= 15)
    {
      if (newName != entry.value)
      {
        _config->set(APP_CONFIG_DEVICE_NAME, newName.c_str());
        if (!_config->save())
        {
          Template::renderStatus("Failed to save device name.", TFT_RED);
          HelperUtility::delayMs(1500);
        }
      }
    } else
    {
      Template::renderStatus("Cannot have more than 15 characters.");
      HelperUtility::delayMs(1500);
    }

    refreshMenu();
  } else if (entry.label == "Navigation Sound")
  {
    _config->set(APP_CONFIG_NAV_SOUND, entry.value == "Off" ? "1" : "0");
    if (!_config->save())
    {
      Template::renderStatus("Failed to save navigation sound.", TFT_RED);
      HelperUtility::delayMs(1500);
    }

    refreshMenu();
  }else if (entry.label == "About")
  {
    renderAbout();
  }
}

void SettingScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}

void SettingScreen::update()
{
  if (currentState == STATE_ABOUT)
  {
    const auto key = &M5Cardputer.Keyboard;
    if (key->isChange() && key->isPressed())
    {
      if (key->isKeyPressed(KEY_BACKSPACE) || key->isKeyPressed('`'))
      {
        init();
      }
    }
  } else
  {
    ListScreen::update();
  }
}