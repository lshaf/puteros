//
// Created by l-sha on 23/10/2025.
//

#include "os/screens/utility/UtilityMenuScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/utility/UtilI2CDetectorScreen.h"

void UtilityMenuScreen::init()
{
  Template::renderHead("Utility Menu");
  setEntries({
    {"I2C Detector"}
  });
}

void UtilityMenuScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "I2C Detector")
  {
    _global->setScreen(new UtilI2CDetectorScreen());
  }
}

void UtilityMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}