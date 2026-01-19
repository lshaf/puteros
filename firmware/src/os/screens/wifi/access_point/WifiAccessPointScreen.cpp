//
// Created by L Shaf on 17/12/25.
//

#include "WifiAccessPointScreen.h"

#include "WiAPFileManagerScreen.h"
#include "os/component/InputTextScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"

void WifiAccessPointScreen::init()
{
  renderMenu();
}

void WifiAccessPointScreen::preRender(M5Canvas& body)
{
  if (isAPCreated())
  {
    body.setTextSize(1);
    body.setTextColor(TFT_WHITE);
    body.drawCenterString(("IP: " + WiFi.softAPIP().toString()).c_str(), body.width() / 2, 0);
  }
}

void WifiAccessPointScreen::renderMenu()
{
  currentState = STATE_MENU;
  Template::renderHead("Access Point");
  if (!isAPCreated())
  {
    const auto config = _global->getConfig();
    const auto ssid = config->get(APP_CONFIG_WIFI_AP_SSID, APP_CONFIG_WIFI_AP_SSID_DEFAULT);
    const auto password = config->get(APP_CONFIG_WIFI_AP_PASSWORD, APP_CONFIG_WIFI_AP_PASSWORD_DEFAULT);
    setEntries({
      {"SSID", ssid.c_str()},
      {"Password", password.c_str()},
      {"Hidden", isHidden ? "Yes" : "No"},
      {"Start"}
    });
  } else
  {
    setEntries({
      {"Wifi QRCode"},
      {"Web File Manager"},
    });
  }
}

void WifiAccessPointScreen::renderQRCode()
{
  currentState = STATE_QR_CODE;
  setEntries({});

  const auto config = _global->getConfig();
  const auto ssid = config->get(APP_CONFIG_WIFI_AP_SSID, APP_CONFIG_WIFI_AP_SSID_DEFAULT);
  const auto password = config->get(APP_CONFIG_WIFI_AP_PASSWORD, APP_CONFIG_WIFI_AP_PASSWORD_DEFAULT);
  const String authMode = password.isEmpty() ? "nopass" : "WPA";
  String qrData = "WIFI:T:" + authMode + ";S:" + ssid;
  if (!password.isEmpty()) qrData += ";P:" + password;
  qrData += ";H:" + String(isHidden ? "true" : "false") + ";;";
  Template::renderQRCode(qrData.c_str());
}

void WifiAccessPointScreen::onEnter(ListEntryItem entry)
{
  if (!isAPCreated())
  {
    if (entry.label == "Hidden")
    {
      isHidden = !isHidden;
      renderMenu();
    } else if (entry.label == "SSID")
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
      WiFi.softAP(ssid.c_str(), password.c_str(), 1, isHidden);
      Template::renderStatus("Access Point Started", TFT_GREEN);
      HelperUtility::delayMs(1500);
      renderMenu();
    }
  } else
  {
    if (entry.label == "Web File Manager")
    {
      _global->setScreen(new WiAPFileManagerScreen());
    } else if (entry.label == "Wifi QRCode")
    {
      renderQRCode();
    }
  }
}



void WifiAccessPointScreen::onBack()
{
  if (currentState == STATE_QR_CODE)
  {
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    renderMenu();
    return;
  }

  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_MODE_STA);
  _global->setScreen(new WifiMenuScreen());
}

void WifiAccessPointScreen::onEscape()
{
  if (currentState == STATE_QR_CODE)
  {
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    renderMenu();
    return;
  }
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_MODE_STA);
  _global->setScreen(new MainMenuScreen());
}
