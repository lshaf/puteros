//
// Created by l-sha on 09/10/2025.
//

#include "os/screens/nfc/NFCMenuScreen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/nfc/NFCPN532Screen.h"
#include "os/screens/nfc/NFCRC522Screen.h"

void NFCMenuScreen::init()
{
  Template::renderHead("NFC");
  setEntries({
    {"RC522 I2C"},
    {"MToolsTech PN532"},
  });
}

void NFCMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}

void NFCMenuScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "MToolsTech PN532")
  {
    _global->setScreen(new NFCPN532Screen());
  } else if (entry.label == "RC522 I2C")
  {
    _global->setScreen(new NFCRC522Screen());
  }
}