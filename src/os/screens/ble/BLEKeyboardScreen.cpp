//
// Created by l-sha on 03/11/2025.
//

#include "os/screens/ble/BLEKeyboardScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/ble/BLEMenuScreen.h"

void BLEKeyboardScreen::init()
{
  const auto batLevel = M5Cardputer.Power.getBatteryLevel();
  bleKeyboard = new BLEKeyboardUtility("PuterOS", "M5Stack", batLevel);
  goMainMenu();
}

void BLEKeyboardScreen::refreshBatteryLevel()
{
  const auto batLevel = M5Cardputer.Power.getBatteryLevel();
  bleKeyboard->setBatteryLevel(batLevel);
}


void BLEKeyboardScreen::goMainMenu()
{
  currentState = STATE_MAIN;
  Template::renderHead("BLE Keyboard");
  setEntries({
    {"Reset Pair"},
    {"Keyboard"},
  });
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
      currentState = STATE_CONNECTING;
      Template::renderHead("BLE Keyboard");
      Template::renderStatus("Waiting for connection...", TFT_BLUE);
      bleKeyboard->begin();
    }
  }
}

void BLEKeyboardScreen::onBack()
{
  if (currentState == STATE_KEYBOARD)
  {
    goMainMenu();
  } else
  {
    _global->setScreen(new BLEMenuScreen());
  }
}

void BLEKeyboardScreen::onEscape()
{
  _global->setScreen(new MainMenuScreen());
}

void BLEKeyboardScreen::update()
{
  if (currentState == STATE_KEYBOARD)
  {
    const auto _keyboard = &M5Cardputer.Keyboard;
    if (_keyboard->isChange() && _keyboard->isPressed())
    {
      const auto s = &_keyboard->keysState();
      if (s->fn)
      {
        if (s->enter)
        {
          bleKeyboard->end();
          goMainMenu();
          return;
        }

        if (s->del)
        {
          bleKeyboard->write(HID_FUNCTION_DELETE);
          return;
        }
      }

      if (s->tab)
      {
        bleKeyboard->write(HID_FUNCTION_TAB);
        return;
      }
      if (s->del)
      {
        bleKeyboard->write(HID_FUNCTION_BACKSPACE);
        return;
      }
      if (s->enter)
      {
        bleKeyboard->write(HID_FUNCTION_ENTER);
        return;
      }

      for (const auto c: s->word)
      {
        if (s->fn)
        {
          Serial.printf("BLEKeyboardScreen::update DEBUG Pressed media key: 0x%04x or %c\n", c, c);
          if (c == ';') bleKeyboard->write(HID_FUNCTION_ARROW_UP);
          else if (c == ',') bleKeyboard->write(HID_FUNCTION_ARROW_LEFT);
          else if (c == '.') bleKeyboard->write(HID_FUNCTION_ARROW_DOWN);
          else if (c == '/') bleKeyboard->write(HID_FUNCTION_ARROW_RIGHT);
          else if (c == '`') bleKeyboard->write(HID_FUNCTION_ESC);
        } else
        {
          bleKeyboard->write(c);
        }
      }
    }
  } else if (currentState == STATE_CONNECTING)
  {
    HelperUtility::delayMs(100);
    const auto _keyboard = &M5Cardputer.Keyboard;
    if (_keyboard->isChange() && _keyboard->isPressed())
    {
      const auto s = &_keyboard->keysState();
      if (s->fn && s->enter)
      {
        bleKeyboard->end();
        goMainMenu();
        return;
      }
    }

    if (bleKeyboard->isConnected())
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
  } else
  {
    ListScreen::update();
  }
}