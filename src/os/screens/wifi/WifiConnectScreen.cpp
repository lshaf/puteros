//
// Created by l-sha on 9/20/2025.
//
#include <WiFi.h>

#include "os/component/Template.hpp"
#include "os/screens/wifi/WifiConnectScreen.h"

#include "os/core/InputScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"

WifiConnectScreen::~WifiConnectScreen()
{
  WiFi.disconnect(true);
}

void WifiConnectScreen::init()
{
  ListScreen::init();
  WiFiClass::mode(WIFI_STA);
  showWifiList();
}

void WifiConnectScreen::showWifiList()
{
  Template::renderHead("Scan WiFi", true);
  std::vector<std::string> wifiList = {};
  const int totalWifi = WiFi.scanNetworks();
  for (int i = 0; i < totalWifi; i++)
  {
    char buffer[51];
    sprintf(buffer, "[%d] %s", WiFi.RSSI(i), WiFi.SSID(i).c_str());
    wifiList.emplace_back(buffer);
  }

  render();
}

void WifiConnectScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}

void WifiConnectScreen::onEnter(const std::string& entry)
{
  if (currentState == STATE_SELECT_WIFI)
  {
    const auto password = InputScreen::popup(entry);
    WiFi.begin(entry.c_str(), password.c_str());
    Template::renderHead("Connecting", true);

    setEntries({});
    render();

    int attempts = 0;
    while (WiFiClass::status() != WL_CONNECTED && attempts < 6)
    {
      delay(500);
      attempts++;
    }

    if (WiFiClass::status() != WL_CONNECTED) {
      showWifiList();
    } else {
      Template::renderHead("Wifi Menu", true);
      setEntries({"Clock", "Exit"});
      currentState = STATE_MENU;
      render();
    }
  }

  if (currentState == STATE_MENU)
  {
    if (entry == "Exit")
    {
      _global->setScreen(new MainMenuScreen());
    }
  }
}

void WifiConnectScreen::update()
{
  ListScreen::update();
}