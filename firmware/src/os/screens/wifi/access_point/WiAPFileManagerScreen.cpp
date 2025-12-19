//
// Created by L Shaf on 17/12/25.
//

#include "WiAPFileManagerScreen.h"

#include "WifiAccessPointScreen.h"
#include "os/component/InputTextScreen.hpp"

#include <WiFi.h>

void WiAPFileManagerScreen::init()
{
  Template::renderHead("Web File Manager");
  if (!_global->getIsSDCardLoaded())
  {
    Template::renderStatus("SD Card is not loaded", TFT_RED);
    HelperUtility::delayMs(2000);
    return;
  }

  currentPassword = _global->getConfig()
    ->get(APP_CONFIG_WIFI_WEB_PASSWORD, APP_CONFIG_WIFI_WEB_PASSWORD_DEFAULT);
  renderMainMenu();
}

void WiAPFileManagerScreen::onBack()
{
  _global->setScreen(new WifiAccessPointScreen());
}

void WiAPFileManagerScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MENU)
  {
    if (entry.label == "Start")
    {
      currentState = STATE_WEB_MANAGER;
      Template::renderStatus("Starting server...");
      if (!server.begin())
      {
        currentState = STATE_MENU;
        Template::renderStatus(server.getError().c_str(), TFT_RED);
        HelperUtility::delayMs(2000);
        renderMainMenu();
        return;
      }

      const auto ipHost = "http://" + WiFi.softAPIP().toString() + "/";
      const auto mdnsHost = "http://puteros.local/";
      Template::renderStatus((ipHost + " or " + mdnsHost).c_str(), TFT_GREEN);
    } else if (entry.label == "Password")
    {
      const auto newPassword = InputTextScreen::popup("Web Password", currentPassword.c_str());
      if (newPassword.empty())
      {
        Template::renderStatus("Password is required");
        HelperUtility::delayMs(2000);
      } else
      {
        auto cleanPassword = String(newPassword.c_str());
        cleanPassword.trim();
        _global->getConfig()->set(APP_CONFIG_WIFI_WEB_PASSWORD, cleanPassword);
        _global->getConfig()->save();
        currentPassword = newPassword.c_str();
      }

      renderMainMenu();
    }
  }
}

void WiAPFileManagerScreen::renderMainMenu()
{
  setEntries({
  {"Password", currentPassword.c_str()},
    {"Start"}
  });
}

void WiAPFileManagerScreen::update()
{
  if (currentState == STATE_WEB_MANAGER)
  {
    const auto _keeb = &M5Cardputer.Keyboard;
    if (_keeb->isChange() && _keeb->isPressed())
    {
      if (_keeb->isKeyPressed(KEY_BACKSPACE) || _keeb->isKeyPressed('`'))
      {
        currentState = STATE_MENU;
        Template::renderStatus("Quit...");
        server.end();
        HelperUtility::delayMs(1000);
        renderMainMenu();
      }
    }
  } else
  {
    ListScreen::update();
  }
}