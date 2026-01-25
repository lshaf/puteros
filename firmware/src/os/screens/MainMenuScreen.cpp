//
// Created by l-sha on 9/20/2025.
//
#include "MainMenuScreen.hpp"

#include "keyboard/KeyboardMenuScreen.h"
#include "FileNavigatorScreen.hpp"
#include "SettingScreen.h"
#include "WelcomeScreen.hpp"
#include "ble/BLEMenuScreen.h"
#include "game/GameMenuScreen.h"
#include "module/ModuleMenuScreen.h"
#include "utility/UtilityMenuScreen.h"
#include "wifi/WifiMenuScreen.h"

void MainMenuScreen::init()
{
  Template::renderHead("Main Menu");
  setEntries({
    {"Wifi"},
    {"Bluetooth"},
    {"Keyboard"},
    {"Files"},
    {"Modules"},
    {"Utility"},
    {"Games"},
    {"Settings"}
  });
}

void MainMenuScreen::onBack()
{
  _global->setScreen(new WelcomeScreen());
}

void MainMenuScreen::onEnter(const ListEntryItem& entry)
{
  if (entry.label == "Wifi")
  {
    _global->setScreen(new WifiMenuScreen());
  } else if (entry.label == "Bluetooth")
  {
    _global->setScreen(new BLEMenuScreen());
  } else if (entry.label == "Keyboard")
  {
    _global->setScreen(new KeyboardMenuScreen());
  } else if (entry.label == "Modules")
  {
    _global->setScreen(new ModuleMenuScreen());
  } else if (entry.label == "Files")
  {
    _global->setScreen(new FileNavigatorScreen("/"));
  } else if (entry.label == "Settings")
  {
    _global->setScreen(new SettingScreen());
  } else if (entry.label == "Utility")
  {
    _global->setScreen(new UtilityMenuScreen());
  } else if (entry.label == "Games")
  {
    _global->setScreen(new GameMenuScreen());
  }
}