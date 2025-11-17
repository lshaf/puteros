//
// Created by l-sha on 04/11/2025.
//

#pragma once
#include "BLEKeyboardUtility.h"

class DuckScriptUtility
{
public:
  explicit DuckScriptUtility(BLEKeyboardUtility* keyboard) : keyboard(keyboard) {};
  explicit DuckScriptUtility(USBHIDKeyboard* keyboard) : keyboardUSB(keyboard) {};
  bool runCommand(const std::string& command);
  uint8_t charToHex(const char* str);
  void runScript(const std::string& pathFile);
  static void runFile(BLEKeyboardUtility* keyboard, const std::string& path);

  void kWrite(uint8_t data)
  {
    if (keyboard)
    {
      keyboard->write(data);
    }
    else if (keyboardUSB)
    {
      keyboardUSB->write(data);
    }
  }

  void kWrite(const uint8_t* data, size_t length)
  {
    if (keyboard)
    {
      keyboard->write(data, length);
    }
    else if (keyboardUSB)
    {
      keyboardUSB->write(data, length);
    }
  }

  void kPress(uint8_t key)
  {
    if (keyboard)
    {
      keyboard->press(key);
    }
    else if (keyboardUSB)
    {
      keyboardUSB->press(key);
    }
  }

  void kReleaseAll()
  {
    if (keyboard)
    {
      keyboard->releaseAll();
    }
    else if (keyboardUSB)
    {
      keyboardUSB->releaseAll();
    }
  }

private:
  BLEKeyboardUtility* keyboard = nullptr;
  USBHIDKeyboard* keyboardUSB = nullptr;
};
