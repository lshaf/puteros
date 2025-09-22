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
      Template::drawStatusBody(("Hello, " + input + "!").c_str());
    }
  }
}

void WelcomeScreen::render()
{
  Template::renderHead(_title, _withBattery);
  Template::drawStatusBody("Press ENTER to Start");
}