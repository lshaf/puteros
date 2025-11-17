//
// Created by l-sha on 17/11/2025.
//

#include "USB.h"
#include "KeyboardUSBScreen.h"

#include "KeyboardMenuScreen.h"
#include "os/utility/BLEKeyboardUtility.h"
#include "os/utility/DuckScriptUtility.h"

void KeyboardUSBScreen::init()
{
  keyboard = new USBHIDKeyboard();
  keyboard->begin();
  USB.begin();
  goMainMenu();
}

void KeyboardUSBScreen::goMainMenu()
{
  currentState = STATE_MENU;
  Template::renderHead("USB Keyboard");
  setEntries({
    {"Emulate Keyboard"},
  });
}

void KeyboardUSBScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "Emulate Keyboard")
  {
    keyboard->releaseAll();
    goConnectedMenu();
  }
}

void KeyboardUSBScreen::onBack()
{
  if (currentState == STATE_MENU)
  {
    onEscape();
  }
}

void KeyboardUSBScreen::onEscape()
{
  esp_restart();
}

void KeyboardUSBScreen::goConnectedMenu()
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

void KeyboardUSBScreen::update()
{
  if (currentState == STATE_KEYBOARD)
  {

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
          Serial.printf("KeyboardUSBScreen::update DEBUG Pressed media key: 0x%04x or %c\n", hid_char, hid_char);
          if (hid_char == ';') report.keys[count] = KEY_UP_ARROW - 0x88;
          else if (hid_char == ',') report.keys[count] = KEY_LEFT_ARROW - 0x88;
          else if (hid_char == '.') report.keys[count] = KEY_DOWN_ARROW - 0x88;
          else if (hid_char == '/') report.keys[count] = KEY_RIGHT_ARROW - 0x88;
          else if (hid_char == '`') report.keys[count] = KEY_ESC - 0x88;
          else if (hid_char == 'z') report.keys[count] = KEY_LEFT_GUI - 0x88;
          else if (c == KEY_BACKSPACE) report.keys[count] = KEY_DELETE - 0x88;
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
              delay(1000);
            }
          }
        } else
        {
          report.keys[count] = c;
        }

        count++;
      }

      keyboard->sendReport(&report);
    } else if (wasPressed)
    {
      wasPressed = false;
      keyboard->releaseAll();
    }
  } else
  {
    ListScreen::update();
  }
}

void KeyboardUSBScreen::runDuckyScript(const std::string& path)
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
  const auto ducky = new DuckScriptUtility(keyboard);
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

void KeyboardUSBScreen::renderPathEntries(const std::string& path)
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