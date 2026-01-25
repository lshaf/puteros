//
// Created by l-sha on 09/10/2025.
//

#include "BLEMenuScreen.h"
#include "BLESpamScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/ble/BLEAnalyzerScreen.h"

void BLEMenuScreen::init()
{
  Template::renderHead("Bluetooth");
  setEntries({
    {"BLE Analyzer"},
    {"BLE Spam"},
  });
}

void BLEMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}

void BLEMenuScreen::onEnter(const ListEntryItem& entry)
{
  if (entry.label == "BLE Analyzer")
  {
    _global->setScreen(new BLEAnalyzerScreen());
  } else if (entry.label == "BLE Spam")
  {
    _global->setScreen(new BLESpamScreen());
  }
}