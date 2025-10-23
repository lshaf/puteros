//
// Created by l-sha on 22/10/2025.
//

#pragma once

#include <MFRC522_I2C.h>
#include "os/core/ListScreen.hpp"

class NFCRC522Screen final : public ListScreen
{
public:
  static constexpr uint8_t I2C_ADDRESS = 0x28;
  void init() override;
  ~NFCRC522Screen() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;

  void goMainMenu();

  void callScanUid();

private:
  enum State_e
  {
    STATE_MAIN_MENU,
    STATE_SCAN_UID,
  } currentState = STATE_MAIN_MENU;
  MFRC522_I2C* _module = nullptr;
};