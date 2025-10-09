//
// Created by l-sha on 09/10/2025.
//

#include "os/screens/nfc/NFCMenuScreen.h"
#include "os/screens/MainMenuScreen.hpp"

void NFCMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}

void NFCMenuScreen::onEnter(ListEntryItem entry)
{
  //
}