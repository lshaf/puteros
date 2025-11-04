//
// Created by l-sha on 04/11/2025.
//

#include <SD.h>

#include "os/utility/DuckScriptUtility.h"
#include "os/utility/HelperUtility.h"

uint8_t DuckScriptUtility::charToHex(const char* str)
{
  // Convert alphanumeric and F1 to F12 keys to their corresponding HID key codes
  uint8_t value = 0;
  size_t strLen = strlen(str);
  if (strLen == 1)
  {
    if (str[0] >= '0' && str[0] <= '9') {
      value = (str[0] - '0') << 4;
    } else if (str[0] >= 'A' && str[0] <= 'F') {
      value = (str[0] - 'A' + 10) << 4;
    } else if (str[0] >= 'a' && str[0] <= 'f') {
      value = (str[0] - 'a' + 10) << 4;
    }
  } else if (strLen == 2 && (str[0] == 'F' || str[0] == 'f'))
  {
    const int fnNumber = std::stoi(std::string(&str[1]));
    if (fnNumber >= 1 && fnNumber <= 12)
    {
      value = static_cast<uint8_t>(0x3A + fnNumber - 1); // F1 starts at 0x3A
    }
  }

  return value;
}


bool DuckScriptUtility::runCommand(const std::string& command)
{
  if (command.rfind("STRING ", 0) == 0)
  {
    const std::string str = command.substr(7);
    keyboard->write(reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
  } else if (command.rfind("DELAY ", 0) == 0)
  {
    const std::string delayStr = command.substr(6);
    const int delayMs = std::stoi(delayStr);
    HelperUtility::delayMs(delayMs);
  } else if (command.rfind("KEYCODE ", 0) == 0)
  {
    const std::string keyStr = command.substr(8);
    keyboard->write(charToHex(keyStr.c_str()));
  } else if (command.rfind("GUI ", 0) == 0)
  {
    keyboard->press(HID_FUNCTION_LEFT_GUI);
    const std::string keyStr = command.substr(4);
    const auto keyCode = charToHex(keyStr.c_str());
    keyboard->press(keyCode);
    keyboard->release(HID_FUNCTION_LEFT_GUI);
    keyboard->release(keyCode);
  } else if (command.rfind("CTRL ", 0) == 0)
  {
    keyboard->press(HID_FUNCTION_LEFT_CTRL);
    const std::string keyStr = command.substr(5);
    const auto keyCode = charToHex(keyStr.c_str());
    keyboard->press(keyCode);
    keyboard->release(HID_FUNCTION_LEFT_CTRL);
    keyboard->release(keyCode);
  } else if (command.rfind("ALT ", 0) == 0)
  {
    keyboard->press(HID_FUNCTION_LEFT_ALT);
    const std::string keyStr = command.substr(4);
    const auto keyCode = charToHex(keyStr.c_str());
    keyboard->press(keyCode);
    keyboard->release(HID_FUNCTION_LEFT_ALT);
    keyboard->release(keyCode);
  } else if (command.rfind("SHIFT ", 0) == 0)
  {
    keyboard->press(HID_FUNCTION_LEFT_SHIFT);
    const std::string keyStr = command.substr(6);
    const auto keyCode = charToHex(keyStr.c_str());
    keyboard->press(keyCode);
    keyboard->release(HID_FUNCTION_LEFT_SHIFT);
    keyboard->release(keyCode);
  } else
  {
    Serial.printf("DuckScriptUtility::runCommand: Unknown command: %s\n", command.c_str());
    return false;
  }

  return true;
}

void DuckScriptUtility::runScript(const std::string& pathFile)
{
  File scriptFile = SD.open(pathFile.c_str());
  if (!scriptFile)
  {
    Serial.println("DuckScriptUtility::runScript: Failed to open script file");
    return;
  }

  while (scriptFile.available())
  {
    String line = scriptFile.readStringUntil('\n');
    line.trim();
    runCommand(line.c_str());
  }

  scriptFile.close();
}

void DuckScriptUtility::runFile(BLEKeyboardUtility* keyboard, const std::string& path)
{
  DuckScriptUtility duckScript(keyboard);
  duckScript.runScript(path);
}