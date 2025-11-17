//
// Created by l-sha on 03/11/2025.
//

#include "os/screens/ble/BLEKeyboardScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/utility/DuckScriptUtility.h"

#include "USBHIDKeyboard.h"

void BLEKeyboardScreen::init()
{
  const auto batLevel = M5Cardputer.Power.getBatteryLevel();
  bleKeyboard = new BLEKeyboardUtility("PuterOS", "M5Stack", batLevel);
  bleKeyboard->begin();

  goMainMenu();

  HelperUtility::makeDirectoryRecursive(duckyScriptPath);
  HelperUtility::makeDirectoryRecursive(shortcutPath);
}

void BLEKeyboardScreen::refreshBatteryLevel()
{
  if (millis() - lastBatteryUpdate >= 60000) return;
  lastBatteryUpdate = millis();
  const auto batLevel = M5Cardputer.Power.getBatteryLevel();
  bleKeyboard->setBatteryLevel(batLevel);
}

void BLEKeyboardScreen::goMainMenu()
{
  currentState = STATE_MAIN;
  Template::renderHead("BLE Keyboard");
  setEntries({
    {"Keyboard"},
    {"Ducky Script"},
    {"Reset Pair"},
  });
}

void BLEKeyboardScreen::goConnectedMenu()
{
  currentState = STATE_KEYBOARD;

  auto body = Template::createBody();
  body.setTextSize(2);
  body.setTextColor(TFT_GREEN);
  body.drawCenterString("Connected", body.width() / 2, body.height() / 2 - body.fontHeight() / 2);

  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.drawCenterString("Press fn+ok to quit", body.width() / 2, body.height() - body.fontHeight() - 2);
  Template::renderBody(&body);
}

void BLEKeyboardScreen::waitingForConnection()
{
  Template::renderStatus("Bluetooth is not connected...", TFT_RED);
  HelperUtility::delayMs(1500);
}


void BLEKeyboardScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MAIN)
  {
    if (entry.label == "Reset Pair")
    {
      bleKeyboard->resetPair();
      Template::renderStatus("Pairing information reset");
      HelperUtility::delayMs(1500);
      goMainMenu();
    } else if (entry.label == "Keyboard")
    {
      bleKeyboard->releaseAll();
      Template::renderHead("BLE Keyboard");
      if (bleKeyboard->isConnected())
        goConnectedMenu();
      else
      {
        waitingForConnection();
        goMainMenu();
      }
    } else if (entry.label == "Ducky Script")
    {
      renderPathEntries(duckyScriptPath);
    }
  } else if (currentState == STATE_SELECT_FILE)
  {
    if (entry.value.empty()) return;
    if (entry.value == "DIR")
    {
      if (currentPath.empty())
        currentPath += entry.label;
      else
        currentPath += "/" + entry.label;

      renderPathEntries(currentPath);
    } else
    {
      std::string fileName;
      if (currentPath.empty())
        fileName = "/" + entry.label;
      else
        fileName = currentPath + "/" + entry.label;

      if (bleKeyboard->isConnected())
        runDuckyScript(fileName);
      else
      {
        waitingForConnection();
        renderPathEntries(currentPath);
      }
    }
  }
}

void BLEKeyboardScreen::onBack()
{
  if (currentState == STATE_KEYBOARD)
  {
    goMainMenu();
  } else if (currentState == STATE_SELECT_FILE)
  {
    if (currentPath == duckyScriptPath)
    {
      goMainMenu();
    } else
    {
      const size_t lastSlash = currentPath.rfind('/');
      if (lastSlash > 0)
        currentPath = currentPath.substr(0, lastSlash);
      else
        currentPath = "";

      renderPathEntries(currentPath);
    }
  } else
  {
    esp_restart();
  }
}

void BLEKeyboardScreen::onEscape()
{
  if (currentState == STATE_SELECT_FILE)
    goMainMenu();
  else
  {
    esp_restart();
  }
}

void BLEKeyboardScreen::update()
{
  if (currentState == STATE_KEYBOARD)
  {
    refreshBatteryLevel();
    const auto _keyboard = &M5Cardputer.Keyboard;
    if (_keyboard->isPressed())
    {
      wasPressed = true;
      const auto s = &_keyboard->keysState();
      KeyReport report = {};

      int count = 0;
      report.modifiers = s->modifiers;
      for (const auto c: s->hid_keys)
      {
        if (s->fn)
        {
          const char hid_char = s->word[count];
          Serial.printf("BLEKeyboardScreen::update DEBUG Pressed media key: 0x%04x or %c\n", hid_char, hid_char);
          if (hid_char == ';') report.keys[count] = KEY_UP_ARROW - HID_OFFSET;
          else if (hid_char == ',') report.keys[count] = KEY_LEFT_ARROW - HID_OFFSET;
          else if (hid_char == '.') report.keys[count] = KEY_DOWN_ARROW - HID_OFFSET;
          else if (hid_char == '/') report.keys[count] = KEY_RIGHT_ARROW - HID_OFFSET;
          else if (hid_char == '`') report.keys[count] = KEY_ESC - HID_OFFSET;
          else if (hid_char == 'z') report.keys[count] = KEY_LEFT_GUI - HID_OFFSET;
          else if (c == KEY_BACKSPACE) report.keys[count] = KEY_DELETE - HID_OFFSET;
          else if (c == KEY_ENTER)
          {
            goMainMenu();
            return;
          }
          else
          {
            std::string filename = shortcutPath + "/" + hid_char + ".ds";
            if (SD.exists(filename.c_str()))
            {
              runDuckyScript(filename);
              goConnectedMenu();
            }
          }
        } else
        {
          report.keys[count] = c;
        }

        count++;
      }

      bleKeyboard->sendReport(&report);
    } else if (wasPressed)
    {
      wasPressed = false;
      bleKeyboard->releaseAll();
    }
    delay(50);
  } else if (currentState == STATE_RUNNING_SCRIPT)
  {
    const auto _keyboard = &M5Cardputer.Keyboard;
    if (_keyboard->isChange() && _keyboard->isPressed())
    {
      if (_keyboard->isKeyPressed(KEY_ENTER) || _keyboard->isKeyPressed('`'))
      {
        printedLines.clear();
        goMainMenu();
      }
    }
  } else
  {
    ListScreen::update();
  }
}

void BLEKeyboardScreen::runDuckyScript(const std::string& path)
{
  currentState = STATE_RUNNING_SCRIPT;
  auto scriptFile = SD.open(path.c_str());
  if (!scriptFile)
  {
    Template::renderStatus("Cannot open file");
    HelperUtility::delayMs(1500);
    renderPathEntries(currentPath);
    return;
  }
  const auto ducky = new DuckScriptUtility(bleKeyboard);
  while (scriptFile.available())
  {
    String line = scriptFile.readStringUntil('\n');
    line.trim();
    if (line.isEmpty()) continue;
    const bool success = ducky->runCommand(line.c_str());
    printedLines.emplace_back(line, success);

    auto body = Template::createBody();
    const size_t maxLines = 11;
    const size_t start = printedLines.size() > maxLines ? printedLines.size() - maxLines : 0;
    for (size_t i = start; i < printedLines.size(); ++i)
    {
      body.setTextColor(printedLines[i].second ? TFT_GREEN : TFT_RED);
      body.drawString(printedLines[i].first, 0, (body.fontHeight() + 1) * (i - start));
    }
    Template::renderBody(&body);
  }

  scriptFile.close();
}


void BLEKeyboardScreen::renderPathEntries(const std::string& path)
{
  if (!_global->getIsSDCardLoaded())
  {
    Template::renderStatus("SD Card is not loaded", TFT_RED);
    HelperUtility::delayMs(1500);
    goMainMenu();
    return;
  }
  currentState = STATE_SELECT_FILE;
  currentPath = path;
  Template::renderHead("Ducky Script");
  auto dir = SD.open(path.c_str());
  std::vector<ListEntryItem> fileEntries;
  while (File currentFile = dir.openNextFile())
  {
    const std::string name = currentFile.name();
    fileEntries.push_back({
      name,
      currentFile.isDirectory() ? "DIR" : "FIL"
    });
    currentFile.close();
  }
  dir.close();

  if (fileEntries.empty())
  {
    Template::renderStatus("No file found");
    HelperUtility::delayMs(1500);
    goMainMenu();
  } else
  {
    setEntries(fileEntries);
  }
}