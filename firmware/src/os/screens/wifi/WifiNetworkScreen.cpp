//
// Created by l-sha on 9/20/2025.
//
#include <WiFi.h>

#include "os/component/Template.hpp"
#include "os/screens/wifi/WifiNetworkScreen.h"

#include "os/component/InputTextScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"
#include "os/screens/wifi/network/WiNetClockScreen.h"
#include "os/screens/wifi/network/WiNetInformationScreen.h"
#include "os/screens/wifi/network/WiNetIPScannerScreen.h"

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
  setEntries({ {"Information"}, {"World Clock"}, {"IP Scanner"} });
}

void WifiNetworkScreen::showWifiList()
{
  Template::renderHead("Scan WiFi", true);
  Template::renderStatus("Scanning...");

  std::vector<ListEntryItem> wifiList = {};
  const int totalWifi = WiFi.scanNetworks();
  for (int i = 0; i < totalWifi; i++)
  {
    char buffer[51];
    sprintf(buffer, "[%d] %s", WiFi.RSSI(i), WiFi.SSID(i).c_str());
    wifiList.push_back({
      buffer,
      WiFi.BSSIDstr(i).c_str()
    });
  }

  setEntries(wifiList);
}

void WifiNetworkScreen::onBack()
{
  WiFi.disconnect(true);
  _global->setScreen(new WifiMenuScreen());
}

void WifiNetworkScreen::onEnter(const ListEntryItem entry)
{
  if (currentState == STATE_SELECT_WIFI)
  {
    setEntries({});

    const std::string ssid = entry.label.substr(entry.label.find(']') + 2);
    const auto password = InputTextScreen::popup(entry.label);
    WiFi.begin(ssid.c_str(), password.c_str());
    Template::renderHead("Connecting");
    Template::renderStatus("Connecting to " + ssid + "...");

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
    if (entry.label == "World Clock")
    {
      _global->setScreen(new WifiConnectClockScreen());
    } else if (entry.label == "Information")
    {
      _global->setScreen(new WiNetInformationScreen());
    } else if (entry.label == "IP Scanner")
    {
      _global->setScreen(new WiNetIPScannerScreen());
    }
  }
}

void WifiNetworkScreen::update()
{
  ListScreen::update();
}