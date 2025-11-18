//
// Created by l-sha on 04/11/2025.
//

#pragma once
#include "HIDKeyboardUtility.h"

class DuckScriptUtility
{
public:
  explicit DuckScriptUtility(HIDKeyboardUtility* keyboard) : keyboard(keyboard) {};
  bool runCommand(const std::string& command);
  uint8_t charToHex(const char* str);
  void runScript(const std::string& pathFile);
  static void runFile(HIDKeyboardUtility* keyboard, const std::string& path);

private:
  HIDKeyboardUtility* keyboard = nullptr;

  void holdPress(uint8_t modifier, uint8_t key);
};

