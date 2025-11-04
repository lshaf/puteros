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
  void init() override;

protected:
  void update() override;
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;

  void refreshBatteryLevel();
  void goMainMenu();
  void goConnectedMenu();
  void waitingForConnection();

  void renderPathEntries(const std::string& path);
  void runDuckyScript(const std::string& path);
private:
  BLEKeyboardUtility* bleKeyboard = nullptr;
  std::string duckyScriptPath = "/puteros/keyboard/duckyscript";
  std::string shortcutPath = "/puteros/keyboard/function";
  std::string currentPath;
  unsigned long lastBatteryUpdate = 0;
  std::vector<std::pair<String, bool>> printedLines;

  enum State_e
  {
    STATE_MAIN,
    STATE_CONNECTING,
    STATE_SELECT_FILE,
    STATE_RUNNING_SCRIPT,
    STATE_KEYBOARD,
  } currentState = STATE_MAIN;
};