//
// Created by l-sha on 17/11/2025.
//

#include "os/screens/keyboard/KeyboardMenuScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/keyboard/KeyboardScreen.h"

void KeyboardMenuScreen::init()
{
  Template::renderHead("Keyboard Menu");
  refreshMenu();
}

void KeyboardMenuScreen::onEnter(const ListEntryItem& entry)
{
  if (entry.label == "Start")
  {
    _global->setScreen(new KeyboardScreen(isUSBMode ? KeyboardScreen::MODE_USB : KeyboardScreen::MODE_BLE));
  } else if (entry.label == "Mode")
  {
    isUSBMode = !isUSBMode;
    refreshMenu(false);
  }
}

void KeyboardMenuScreen::refreshMenu(const bool reset)
{
  std::vector<ListEntryItem> f = {
    {"Mode", isUSBMode ? "USB" : "BLE"},
    {"Start"},
  };

  if (reset) setEntries(f);
  else
  {
    entries = f;
    render();
  }
}


void KeyboardMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}
