//
// Created by l-sha on 9/20/2025.
//

#include "os/screens/WelcomeScreen.hpp"

#include "os/core/InputScreen.hpp"
#include "os/core/ScreenManager.hpp"
#include "os/screens/MainMenuScreen.hpp"

WelcomeScreen::WelcomeScreen()
{
  _header.setTextColor(TFT_WHITE);
  _header.setTextSize(1.5);
  _header.drawString("PuterOS", 2, _header.height() / 2 - _header.fontHeight() / 2);

  _body.setTextColor(TFT_WHITE);
  _body.drawCenterString("Press ENTER to Start", _body.width() / 2, _body.height() / 2 - _body.fontHeight() / 2);

  render();
}


void WelcomeScreen::update()
{
  refreshBattery();
  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    if (_keyboard->isKeyPressed(KEY_ENTER))
    {
      ScreenManager::to(new MainMenuScreen());
    }

    if (_keyboard->isKeyPressed(KEY_BACKSPACE))
    {
      const auto input = InputScreen::popup("Your Name");
      _body.fillSprite(BLACK);
      _body.setTextColor(TFT_WHITE);
      _body.drawCenterString(("Hello, " + input + "!").c_str(), _body.width() / 2, _body.height() / 2 - _body.fontHeight() / 2);
      render(RENDER_BODY);
    }
  }
}