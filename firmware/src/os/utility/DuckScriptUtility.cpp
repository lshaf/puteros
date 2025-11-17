//
// Created by l-sha on 04/11/2025.
//

#include <SD.h>

#include "DuckScriptUtility.h"

uint8_t DuckScriptUtility::charToHex(const char* str)
{
  // Convert alphanumeric and F1 to F12 keys to their corresponding HID key codes
  uint8_t value = 0;
  size_t strLen = strlen(str);
  if (strLen == 1)
  {
    value = static_cast<uint8_t>(str[0]);
  } else if (strLen == 2 && (str[0] == 'F' || str[0] == 'f'))
  {
    const int fnNumber = std::stoi(std::string(&str[1]));
    if (fnNumber >= 1 && fnNumber <= 12)
    {
      value = static_cast<uint8_t>(0xC1 + fnNumber); // F1 starts at 0x3A
    }
  }

  return value;
}


bool DuckScriptUtility::runCommand(const std::string& command)
{
  if (command.rfind("STRING ", 0) == 0)
  {
    const std::string str = command.substr(7);
    kWrite(reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
  } else if (command.rfind("STRINGLN ", 0) == 0)
  {
    const std::string str = command.substr(9);
    kWrite(reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
    kWrite(KEY_RETURN);
  } else if (command.rfind("DELAY ", 0) == 0)
  {
    const std::string delayStr = command.substr(6);
    const int delayMs = std::stoi(delayStr);
    delay(delayMs);
  } else if (command.rfind("GUI ", 0) == 0)
  {
    const std::string keyStr = command.substr(4);
    const auto keyCode = charToHex(keyStr.c_str());
    kPress(KEY_LEFT_GUI);
    kPress(keyCode);
    kReleaseAll();
  } else if (command.rfind("CTRL ", 0) == 0)
  {
    const std::string keyStr = command.substr(5);
    const auto keyCode = charToHex(keyStr.c_str());
    kPress(KEY_LEFT_CTRL);
    kPress(keyCode);
    kReleaseAll();
  } else if (command.rfind("ALT ", 0) == 0)
  {
    const std::string keyStr = command.substr(4);
    const auto keyCode = charToHex(keyStr.c_str());
    kPress(KEY_LEFT_ALT);
    kPress(keyCode);
    kReleaseAll();
  } else if (command.rfind("SHIFT ", 0) == 0)
  {
    const std::string keyStr = command.substr(6);
    const auto keyCode = charToHex(keyStr.c_str());
    kPress(KEY_LEFT_SHIFT);
    kPress(keyCode);
    kReleaseAll();
  } else if (command.rfind("ENTER", 0) == 0)
  {
    kWrite(KEY_RETURN);
  } else if (command.rfind("REM ", 0) == 0)
  {
    // nothing to do here
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
    if (line.isEmpty()) continue;
    runCommand(line.c_str());
  }

  scriptFile.close();
}

void DuckScriptUtility::runFile(BLEKeyboardUtility* keyboard, const std::string& path)
{
  DuckScriptUtility duckScript(keyboard);
  duckScript.runScript(path);
}