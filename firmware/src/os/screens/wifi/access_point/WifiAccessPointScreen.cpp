//
// Created by L Shaf on 17/12/25.
//

#include "WifiAccessPointScreen.h"
#include <WiFi.h>

#include "WiAPFileManagerScreen.h"
#include "os/component/InputTextScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"

void WifiAccessPointScreen::init()
{
  renderMenu();
}

void WifiAccessPointScreen::renderMenu()
{
  Template::renderHead("Access Point");
  if (WiFi.getMode() != WIFI_MODE_AP && WiFi.getMode() != WIFI_MODE_APSTA)
  {
    const auto config = _global->getConfig();
    const auto ssid = config->get(APP_CONFIG_WIFI_AP_SSID, APP_CONFIG_WIFI_AP_SSID_DEFAULT);
    const auto password = config->get(APP_CONFIG_WIFI_AP_PASSWORD, APP_CONFIG_WIFI_AP_PASSWORD_DEFAULT);
    setEntries({
      {"SSID", ssid.c_str()},
      {"Password", password.c_str()},
      {"Start"}
    });
  } else
  {
    setEntries({
      {"IP Address", WiFi.softAPIP().toString().c_str()},
      {"Web File Manager"},
    });
  }
}

void WifiAccessPointScreen::onEnter(ListEntryItem entry)
{
  if (WiFi.getMode() != WIFI_MODE_AP && WiFi.getMode() != WIFI_MODE_APSTA)
  {
    if (entry.label == "SSID")
    {
      const auto ssid = _global->getConfig()->get(APP_CONFIG_WIFI_AP_SSID, APP_CONFIG_WIFI_AP_SSID_DEFAULT);
      const auto newSsid = InputTextScreen::popup("SSID", ssid.c_str());
      if (newSsid.empty())
      {
        Template::renderStatus("SSID is required", TFT_RED);
        HelperUtility::delayMs(1500);
        renderMenu();
        return;
      }

      if (newSsid.length() > 32)
      {
        Template::renderStatus("SSID too long", TFT_RED);
        HelperUtility::delayMs(1500);
        renderMenu();
        return;
      }

      _global->getConfig()->set(APP_CONFIG_WIFI_AP_SSID, newSsid.c_str());
      _global->getConfig()->save();
      renderMenu();
    } else if (entry.label == "Password")
    {
      const auto password = _global->getConfig()->get(APP_CONFIG_WIFI_AP_PASSWORD, APP_CONFIG_WIFI_AP_PASSWORD_DEFAULT);
      const auto newPassword = InputTextScreen::popup("Password", password.c_str());
      if (newPassword.length() > 0 && newPassword.length() < 8)
      {
        Template::renderStatus("Password too short or leave it empty for no password", TFT_RED);
        HelperUtility::delayMs(1500);
        renderMenu();
        return;
      }

      if (newPassword.length() > 63)
      {
        Template::renderStatus("Password too long", TFT_RED);
        HelperUtility::delayMs(1500);
        renderMenu();
        return;
      }

      _global->getConfig()->set(APP_CONFIG_WIFI_AP_PASSWORD, newPassword.c_str());
      _global->getConfig()->save();
      renderMenu();
    } else if (entry.label == "Start")
    {
      const auto config = _global->getConfig();
      const auto ssid = config->get(APP_CONFIG_WIFI_AP_SSID, APP_CONFIG_WIFI_AP_SSID_DEFAULT);
      const auto password = config->get(APP_CONFIG_WIFI_AP_PASSWORD, APP_CONFIG_WIFI_AP_PASSWORD_DEFAULT);

      WiFi.mode(WIFI_MODE_AP);
      WiFi.softAPConfig(
        IPAddress{10, 0, 0, 1},
        IPAddress{10, 0, 0, 1},
        IPAddress{255, 255, 255, 0}
      );
      WiFi.softAP(ssid.c_str(), password.c_str());
      Template::renderStatus("Access Point Started", TFT_GREEN);
      HelperUtility::delayMs(1500);
      renderMenu();
    }
  } else
  {
    if (entry.label == "Web File Manager")
    {
      _global->setScreen(new WiAPFileManagerScreen());
    }
  }
}

void WifiAccessPointScreen::onBack()
{
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_MODE_STA);
  _global->setScreen(new WifiMenuScreen());
}

void WifiAccessPointScreen::onEscape()
{
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_MODE_STA);
  _global->setScreen(new MainMenuScreen());
}
