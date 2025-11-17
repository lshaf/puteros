//
// Created by l-sha on 22/10/2025.
//

#pragma once

#include <MFRC522_I2C.h>
#include <unordered_map>
#include "os/core/ListScreen.hpp"
#include "os/utility/NFCUtility.h"

class ModuleMFRC522Screen final : public ListScreen
{
public:
  static constexpr uint8_t I2C_ADDRESS = 0x28;
  void init() override;
  ~ModuleMFRC522Screen() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;

  void goMainMenu();
  void goMifareClassic();
  void goShowDiscoveredKeys();

  void callScanUid();
  void callAuthenticate();
  void callMemoryReader();

  bool resetCardState();
  std::string cUidToString(byte* uid, byte uidLen)
  {
    std::string uidStr;
    for (byte i = 0; i < uidLen; i++)
    {
      char line[3];
      sprintf(line, "%02X", uid[i]);
      uidStr += line;
    }
    return uidStr;
  }
private:
  MFRC522_I2C::Uid _currentCard = {};
  std::array<std::pair<NFCUtility::MIFARE_Key, NFCUtility::MIFARE_Key>, 40> _mf1AuthKeys;
  std::unordered_map<MFRC522_I2C::PICC_Type, std::pair<size_t, size_t>> _mf1CardDetails = {
    { MFRC522_I2C::PICC_TYPE_MIFARE_MINI, { 5, 20 } },
    { MFRC522_I2C::PICC_TYPE_MIFARE_1K,   { 16, 64 } },
    { MFRC522_I2C::PICC_TYPE_MIFARE_4K,   { 40, 256 } }
  };

  enum State_e
  {
    STATE_MAIN_MENU,
    STATE_SCAN_UID,
    STATE_AUTHENTICATE,
    STATE_MIFARE_CLASSIC,
    STATE_SHOW_KEY,
    STATE_MEMORY_READER,
  } currentState = STATE_MAIN_MENU;
  MFRC522_I2C _module{I2C_ADDRESS, static_cast<byte>(-1)};
};