//
// Created by l-sha on 9/20/2025.
//

#include "os/component/Template.hpp"
#include "os/screens/WelcomeScreen.hpp"

#include "os/core/InputScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"

void WelcomeScreen::update()
{
  const auto _keyboard = &M5Cardputer.Keyboard;
  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    if (_keyboard->isKeyPressed(KEY_ENTER))
    {
      _global->setScreen(new MainMenuScreen());
    }

    if (_keyboard->isKeyPressed('`'))
    {
      const auto input = InputScreen::popup("Your Name");
      auto body = Template::createBody();
      body.setTextColor(TFT_WHITE);
      body.drawCenterString(("Hello, " + input + "!").c_str(), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
      Template::renderBody(&body);
    }
  }
}

void WelcomeScreen::render()
{
  Template::renderHead(_title, _withBattery);

  M5Canvas body;
  const auto size = Template::bodySize();
  body.createSprite(size.width, size.height);
  body.setTextColor(TFT_WHITE);
  body.drawCenterString("Press ENTER to Start", body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}