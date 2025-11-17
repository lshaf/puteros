//
// Created by l-sha on 17/11/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"
#include "USBHIDKeyboard.h"

class KeyboardUSBScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;

  void goMainMenu();
  void goConnectedMenu();

  void renderPathEntries(const std::string& path);
  void runDuckyScript(const std::string& path);
private:
  std::string duckyScriptPath = "/puteros/keyboard/duckyscript";
  std::string shortcutPath = "/puteros/keyboard/function";
  std::string currentPath;

  bool wasPressed = false;
  std::vector<std::pair<String, bool>> printedLines;
  USBHIDKeyboard* keyboard = nullptr;

  enum State_e
  {
    STATE_KEYBOARD,
    STATE_MENU,
    STATE_RUNNING_SCRIPT,
    STATE_SELECT_FILE,
  } currentState = STATE_MENU;
};