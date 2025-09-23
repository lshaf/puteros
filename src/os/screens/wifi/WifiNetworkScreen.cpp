//
// Created by l-sha on 9/20/2025.
//
#include <WiFi.h>

#include "os/component/Template.hpp"
#include "os/screens/wifi/WifiNetworkScreen.h"

#include "os/core/InputScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"
#include "os/screens/wifi/connect/WiNetClockScreen.h"
#include "os/screens/wifi/connect/WiNetInformationScreen.h"

void WifiNetworkScreen::init()
{
  WiFi.mode(WIFI_STA);
  if (WiFi.status() == WL_CONNECTED)
  {
    showMenu();
  } else
  {
    showWifiList();
  }
}

void WifiNetworkScreen::showMenu()
{
  currentState = STATE_MENU;
  Template::renderHead("Network");
  setEntries({ "Information", "Clock" });
}

void WifiNetworkScreen::showWifiList()
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

void WifiNetworkScreen::onBack()
{
  WiFi.disconnect(true);
  _global->setScreen(new WifiMenuScreen());
}

void WifiNetworkScreen::onEnter(const std::string& entry)
{
  if (currentState == STATE_SELECT_WIFI)
  {
    setEntries({});

    const std::string ssid = entry.substr(entry.find(']') + 2);
    const auto password = InputScreen::popup(entry);
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
      onBack();
    } else {
      showMenu();
    }
  } else if (currentState == STATE_MENU)
  {
    if (entry == "Clock")
    {
      _global->setScreen(new WifiConnectClockScreen());
    } else if (entry == "Information")
    {
      _global->setScreen(new WiNetInformationScreen());
    }
  }
}

void WifiNetworkScreen::update()
{
  ListScreen::update();
}