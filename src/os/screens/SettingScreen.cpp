//
// Created by l-sha on 9/23/2025.
//

#include "os/screens/SettingScreen.h"

#include "os/component/InputNumberScreen.h"
#include "os/screens/MainMenuScreen.hpp"

void SettingScreen::init()
{
  currentState = STATE_MAIN;
  const String brightnessValue = _config->get(CONFIG_BRIGHTNESS, "90");
  Template::renderHead("Settings");
  setEntries({{"Brightness", brightnessValue.c_str()}, {"About", ""}});
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
      _config->set(CONFIG_BRIGHTNESS, String(newBrightness));
      const bool saved = _config->save();
      if (saved) M5Cardputer.Lcd.setBrightness(newBrightness / 100.0 * 255);
    }

    init();
  } else if (entry.label == "About")
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