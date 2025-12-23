//
// Created by l-sha on 9/20/2025.
//
#include <WiFi.h>

#include "os/component/Template.hpp"
#include "WifiNetworkScreen.h"

#include "WiNetFileManager.h"
#include "os/component/InputTextScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"
#include "os/screens/wifi/network/WiNetClockScreen.h"
#include "os/screens/wifi/network/WiNetInformationScreen.h"
#include "os/screens/wifi/network/WiNetIPScannerScreen.h"

void WifiNetworkScreen::init()
{
  WiFi.mode(WIFI_MODE_STA);
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
  setEntries({
    {"Information"},
    {"World Clock"},
    {"IP Scanner"},
    {"Web File Manager"}
  });
}

void WifiNetworkScreen::showWifiList()
{
  Template::renderHead("Scan WiFi", true);
  Template::renderStatus("Scanning...");

  std::vector<ListEntryItem> wifiList = {};
  WiFi.scanDelete();
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
  WiFi.disconnect(true, true);
  Template::renderStatus("Disconnecting...");
  HelperUtility::delayMs(1000);
  _global->setScreen(new WifiMenuScreen());
}

String WifiNetworkScreen::passwordWifiPath(const String& bssid, const String& ssid)
{
  auto cleanBssid = bssid;
  cleanBssid.replace(":", "");
  HelperUtility::makeDirectoryRecursive(passwordPath.c_str());
  return passwordPath + "/" + cleanBssid.c_str() + "_" + ssid.c_str() + ".pass";
}

bool WifiNetworkScreen::connectToWifi(const String& bssid, const String& ssid, const String& password)
{
  Template::renderHead("Connecting");
  Template::renderStatus(("Connecting to " + ssid + "...").c_str());
  WiFi.begin(ssid, password);

  const uint8_t res = WiFi.waitForConnectResult(10000);
  if (res == WL_CONNECTED)
  {
    if (_global->getIsSDCardLoaded())
    {
      const auto path = passwordWifiPath(bssid, ssid);
      File passwordFile = SD.open(path, FILE_WRITE);
      if (passwordFile)
      {
        passwordFile.print(password);
        passwordFile.close();
      }
    }

    showMenu();
    return true;
  } else if (res == WL_CONNECT_FAILED)
  {
    Template::renderStatus("Connection Failed!");
    HelperUtility::delayMs(1000);
  } else if (res == WL_NO_SSID_AVAIL)
  {
    Template::renderStatus("SSID Not Available!");
    HelperUtility::delayMs(1000);
  } else
  {
    Template::renderStatus("Connection Error!");
    HelperUtility::delayMs(1000);
  }
  return false;
}

void WifiNetworkScreen::onEnter(const ListEntryItem entry)
{
  if (currentState == STATE_SELECT_WIFI)
  {
    setEntries({});

    const std::string ssid = entry.label.substr(entry.label.find(']') + 2);
    if (_global->getIsSDCardLoaded())
    {
      const auto path = passwordWifiPath(entry.value.c_str(), ssid.c_str());
      if (SD.exists(path))
      {
        File passwordFile = SD.open(path, FILE_READ);
        if (passwordFile)
        {
          const auto password = passwordFile.readString();
          passwordFile.close();

          const bool isConnected = connectToWifi(entry.value.c_str(), ssid.c_str(), password);
          if (isConnected) return;
        }
      }
    }

    const auto password = InputTextScreen::popup(entry.label);
    const bool isConnected = connectToWifi(entry.value.c_str(), ssid.c_str(), password.c_str());
    if (!isConnected) onBack();
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
    } else if (entry.label == "Web File Manager")
    {
      _global->setScreen(new WiNetFileManager());
    }
  }
}

void WifiNetworkScreen::update()
{
  ListScreen::update();
}