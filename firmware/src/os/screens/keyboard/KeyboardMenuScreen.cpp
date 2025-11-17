//
// Created by l-sha on 17/11/2025.
//

#include "os/screens/keyboard/KeyboardMenuScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/keyboard/KeyboardUSBScreen.h"

void KeyboardMenuScreen::init()
{
  Template::renderHead("Keyboard Menu");
  setEntries({
    {"Mode", "USB"},
    {"Start"},
  });
}

void KeyboardMenuScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "Start")
  {
    _global->setScreen(new KeyboardUSBScreen());
  }
}

void KeyboardMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}
