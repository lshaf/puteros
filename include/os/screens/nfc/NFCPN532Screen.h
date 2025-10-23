//
// Created by L Shaf on 19/10/25.
//

#pragma once

#include <unordered_map>
#include "os/core/ListScreen.hpp"
#include "os/utility/ExtendedPN532Killer.h"
#include "os/utility/NFCUtility.h"

class NFCPN532Screen final : public ListScreen
{
public:
  void init() override;
  ~NFCPN532Screen() override;
protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;

private:
  // Common/default MIFARE Classic 6-byte keys
  TagTechnology::Iso14aTagInfo _currentCard = {};
  std::array<std::pair<NFCUtility::MIFARE_Key, NFCUtility::MIFARE_Key>, 40> _mf1AuthKeys;
  std::unordered_map<ExtendedPN532Killer::CardType, std::pair<size_t, size_t>> _mf1CardDetails = {
    { ExtendedPN532Killer::CARD_TYPE_MIFARE_MINI, { 5, 20 } },
    { ExtendedPN532Killer::CARD_TYPE_MIFARE_1K,   { 16, 64 } },
    { ExtendedPN532Killer::CARD_TYPE_MIFARE_4K,   { 40, 256 } }
  };


  ExtendedPN532Killer _module = ExtendedPN532Killer(Serial1);
  bool _isKiller = false;
  enum State_e
  {
    STATE_PN532,
    STATE_PN532_KILLER,
    STATE_SCAN_UID,
    STATE_MIFARE_CLASSIC,
    STATE_SHOW_KEY,
    STATE_AUTHENTICATE,
    STATE_MEMORY_READER
  } currentState = STATE_PN532;

  void goPN532Menu();
  void goPN532KillerMenu();
  void goMifareClassicMenu();
  void goActualMenu();
  void goShowDiscoveredKeys();

  void callScanUid();
  void callMemoryReader();
  void callAuthenticate();
};
