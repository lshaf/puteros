//
// Created by L Shaf on 19/10/25.
//

#pragma once
#include "os/core/ListScreen.hpp"
#include "os/utility/ExtendedPN532Killer.h"

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
  std::array<ExtendedPN532Killer::MfcKey, 9> _defaultKeys = {{
    ExtendedPN532Killer::MfcKey{ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, // FFFFFFFFFFFF
    ExtendedPN532Killer::MfcKey{ 0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5 }, // A0A1A2A3A4A5
    ExtendedPN532Killer::MfcKey{ 0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5 }, // B0B1B2B3B4B5
    ExtendedPN532Killer::MfcKey{ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }, // 000000000000
    ExtendedPN532Killer::MfcKey{ 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF }, // AABBCCDDEEFF
    ExtendedPN532Killer::MfcKey{ 0x4D, 0x3A, 0x99, 0xC3, 0x51, 0xDD }, // 4D3A99C351DD
    ExtendedPN532Killer::MfcKey{ 0x1A, 0x98, 0x2C, 0x7E, 0x45, 0x9A }, // 1A982C7E459A
    ExtendedPN532Killer::MfcKey{ 0xD3, 0xF7, 0xD3, 0xF7, 0xD3, 0xF7 }, // D3F7D3F7D3F7
    ExtendedPN532Killer::MfcKey{ 0x71, 0x4C, 0x5C, 0x88, 0x05, 0x92 }  // 714C5C880592
  }};

  ExtendedPN532Killer _module = ExtendedPN532Killer(Serial1);
  bool _isKiller = false;
  enum State_e
  {
    STATE_PN532,
    STATE_PN532_KILLER,
    STATE_SCAN_UID,
    STATE_MEMORY_READER
  } currentState = STATE_PN532;

  void goPN532Menu();
  void goPN532KillerMenu();
  void goActualMenu();

  void callScanUid();
  void callMemoryReader();
};
