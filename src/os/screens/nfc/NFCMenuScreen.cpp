//
// Created by l-sha on 09/10/2025.
//

#include "os/screens/nfc/NFCMenuScreen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/nfc/NFCPN532Screen.h"

void NFCMenuScreen::init()
{
  Template::renderHead("NFC");
  setEntries({
    {"M5 RFID"},
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
  }
}