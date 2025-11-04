//
// Created by l-sha on 03/11/2025.
//
#pragma once

#include <esp_system.h>
#include "os/core/ListScreen.hpp"
#include "os/utility/BLEKeyboardUtility.h"

class BLEKeyboardScreen final : public ListScreen
{
public:
  ~BLEKeyboardScreen() override
  {
    esp_restart();
  }

  void init() override;

protected:
  void update() override;
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;

  void refreshBatteryLevel();
  void goMainMenu();

private:
  BLEKeyboardUtility* bleKeyboard = nullptr;
  enum State_e
  {
    STATE_MAIN,
    STATE_CONNECTING,
    STATE_KEYBOARD,
  } currentState = STATE_MAIN;
};