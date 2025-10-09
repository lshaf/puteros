//
// Created by l-sha on 09/10/2025.
//

#include "os/screens/ble/BLEMenuScreen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/ble/BLEAnalyzerScreen.h"

void BLEMenuScreen::init()
{
  Template::renderHead("Bluetooth");
  setEntries({
    {"BLE Analyzer"},
    {"BLE Spam", "In Progress"},
  });
}

void BLEMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}

void BLEMenuScreen::onEnter(const ListEntryItem entry)
{
  if (entry.label == "BLE Analyzer")
  {
    _global->setScreen(new BLEAnalyzerScreen());
  }
}