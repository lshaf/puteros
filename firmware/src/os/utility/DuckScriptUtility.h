//
// Created by l-sha on 04/11/2025.
//

#pragma once
#include "BLEKeyboardUtility.h"

class DuckScriptUtility
{
public:
  explicit DuckScriptUtility(BLEKeyboardUtility* keyboard) : keyboard(keyboard) {};
  bool runCommand(const std::string& command);
  uint8_t charToHex(const char* str);
  void runScript(const std::string& pathFile);
  static void runFile(BLEKeyboardUtility* keyboard, const std::string& path);

private:
  BLEKeyboardUtility* keyboard;
};
