//
// Created by l-sha on 09/10/2025.
//

#include "os/screens/module/ModuleMenuScreen.h"

#include "ModuleCapLoraScreen.h"
#include "ModuleGPSScreen.h"
#include "ModuleInfraredScreen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/module/ModulePN532Screen.h"
#include "os/screens/module/ModuleMFRC522Screen.h"

void ModuleMenuScreen::init()
{
  Template::renderHead("Modules");
  setEntries({
    {"RC522 I2C"},
    {"MToolsTech PN532"},
    {"Unit GPS v1.1"},
    {"Cap Lora-1262"},
    {"Unit IR"}
  });
}

void ModuleMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}

void ModuleMenuScreen::onEnter(const ListEntryItem& entry)
{
  if (entry.label == "MToolsTech PN532")
  {
    _global->setScreen(new ModulePN532Screen());
  } else if (entry.label == "RC522 I2C")
  {
    _global->setScreen(new ModuleMFRC522Screen());
  } else if (entry.label == "Unit GPS v1.1")
  {
    _global->setScreen(new ModuleGPSScreen());
  } else if (entry.label == "Cap Lora-1262")
  {
    _global->setScreen(new ModuleCapLoraScreen());
  } else if (entry.label == "Unit IR")
  {
    _global->setScreen(new ModuleInfraredScreen());
  }
}