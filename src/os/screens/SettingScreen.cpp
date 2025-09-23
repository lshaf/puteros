//
// Created by l-sha on 9/23/2025.
//

#include "os/screens/SettingScreen.h"

#include "os/component/InputNumberScreen.h"
#include "os/screens/MainMenuScreen.hpp"

void SettingScreen::init()
{
  currentState = STATE_MAIN;
  Template::renderHead("Settings");
  setEntries({"Brightness", "About"});
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
  body.drawCenterString("Powered by M5Stack", body.width() / 2, body.height() / 2 + body.fontHeight());
  Template::renderBody(&body);
}

void SettingScreen::onEnter(const std::string& entry)
{
  if (entry == "Brightness")
  {
    const auto config = _global->getConfig();
    const auto brightnessKey = "brightness";
    const int currentBrightness = config->get(brightnessKey, "90").toInt();
    const int newBrightness = InputNumberScreen::popup("Brightness", currentBrightness, 0, 100);
    if (newBrightness != currentBrightness)
    {
      config->set(brightnessKey, String(newBrightness));
      const bool saved = config->save();
      if (saved) M5Cardputer.Lcd.setBrightness(newBrightness / 100.0 * 255);
    }

    render();
  } else if (entry == "About")
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