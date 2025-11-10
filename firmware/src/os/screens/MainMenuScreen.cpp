//
// Created by l-sha on 9/20/2025.
//
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/SettingScreen.h"
#include "os/screens/WelcomeScreen.hpp"
#include "os/screens/ble/BLEMenuScreen.h"
#include "os/screens/game/GameMenuScreen.h"
#include "os/screens/module/ModuleMenuScreen.h"
#include "os/screens/utility/UtilityMenuScreen.h"
#include "os/screens/wifi/WifiMenuScreen.h"

void MainMenuScreen::init()
{
  Template::renderHead("Main Menu");
  setEntries({
    {"Wifi"},
    {"Bluetooth"},
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

void MainMenuScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "Wifi")
  {
    _global->setScreen(new WifiMenuScreen());
  } else if (entry.label == "Bluetooth")
  {
    _global->setScreen(new BLEMenuScreen());
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