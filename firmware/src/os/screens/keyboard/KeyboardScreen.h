//
// Created by l-sha on 17/11/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"
#include "os/utility/HIDKeyboardUtility.h"

class KeyboardScreen final : public ListScreen
{
public:
  static constexpr int MODE_USB = 0;
  static constexpr int MODE_BLE = 1;
  explicit KeyboardScreen(int mode);
  void init() override;

protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;

  void goMainMenu();
  void goConnectedMenu();
  void waitingForConnection();
  void refreshBatteryLevel();

  void renderPathEntries(const std::string& path);
  void runDuckyScript(const std::string& path);
private:
  std::string duckyScriptPath = "/m5geek/keyboard/duckyscript";
  std::string shortcutPath = "/m5geek/keyboard/function";
  std::string currentPath;
  int mode = MODE_USB;

  bool wasPressed = false;
  std::vector<std::pair<String, bool>> printedLines;
  HIDKeyboardUtility* keyboard = nullptr;
  unsigned long lastBatteryUpdate = 0;

  enum State_e
  {
    STATE_KEYBOARD,
    STATE_MENU,
    STATE_RUNNING_SCRIPT,
    STATE_SELECT_FILE,
  } currentState = STATE_MENU;
};
