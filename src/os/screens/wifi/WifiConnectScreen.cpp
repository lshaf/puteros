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
  WiFi.mode(WIFI_STA);
  showWifiList();
}

void WifiConnectScreen::showWifiList()
{
  Template::renderHead("Scan WiFi", true);
  Template::drawStatusBody("Scanning...");

  std::vector<std::string> wifiList = {};
  const int totalWifi = WiFi.scanNetworks();
  for (int i = 0; i < totalWifi; i++)
  {
    char buffer[51];
    sprintf(buffer, "[%d] %s", WiFi.RSSI(i), WiFi.SSID(i).c_str());
    wifiList.emplace_back(buffer);
  }

  setEntries(wifiList);
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
    setEntries({});

    const std::string ssid = entry.substr(entry.find(']') + 2);
    const auto password = InputScreen::popup(entry.c_str());
    WiFi.begin(ssid.c_str(), password.c_str());
    Template::renderHead("Connecting");
    Template::drawStatusBody("Connecting to " + ssid + "...");

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 6)
    {
      delay(500);
      attempts++;
    }

    if (WiFi.status() != WL_CONNECTED) {
      _global->setScreen(new MainMenuScreen());
    } else {
      Template::renderHead("Wifi Menu");
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