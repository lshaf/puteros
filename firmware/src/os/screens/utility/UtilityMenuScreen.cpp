//
// Created by l-sha on 23/10/2025.
//

#include "os/screens/utility/UtilityMenuScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/utility/UtilityI2CDetectorScreen.h"
#include "os/screens/utility/UtilityQRCodeScreen.h"

void UtilityMenuScreen::init()
{
  Template::renderHead("Utility Menu");
  setEntries({
    {"I2C Detector"},
    {"QRCode"}
  });
}

void UtilityMenuScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "I2C Detector")
  {
    _global->setScreen(new UtilityI2CDetectorScreen());
  } else if (entry.label == "QRCode")
  {
    _global->setScreen(new UtilityQRCodeScreen());
  }
}

void UtilityMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}