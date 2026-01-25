//
// Created by l-sha on 9/26/2025.
//
#include <WiFi.h>

#include "os/screens/wifi/WifiAnalyzerScreen.h"

#include "os/screens/wifi/WifiMenuScreen.h"

void WifiAnalyzerScreen::init()
{
  Template::renderHead("WiFi Analyzer");
  Template::renderStatus("Scanning...");

  grabWifiList();
}

void WifiAnalyzerScreen::grabWifiList()
{
  WiFi.mode(WIFI_STA);
  const int totalWifi = WiFi.scanNetworks(false, true);
  for (int i = 0; i < totalWifi; ++i) {
    WifiDetail_t detail;
    auto rssi = WiFi.RSSI(i);
    auto rssiStrength = "Very Low";
    if (rssi >= -50) rssiStrength = "Very Good";
    else if (rssi >= -60) rssiStrength = "Good";
    else if (rssi >= -70) rssiStrength = "Better";
    else if (rssi >= -80) rssiStrength = "Low";
    detail.rssi = "["+ String(rssi) +"] " + rssiStrength;
    detail.ssid = WiFi.SSID(i);
    detail.bssid = WiFi.BSSIDstr(i);
    detail.channel = WiFi.channel(i);
    switch (WiFi.encryptionType(i)) {
      case WIFI_AUTH_OPEN: detail.encryptionType = "OPEN"; break;
      case WIFI_AUTH_WEP: detail.encryptionType = "WEP"; break;
      case WIFI_AUTH_WPA_PSK: detail.encryptionType = "WPA_PSK"; break;
      case WIFI_AUTH_WPA2_PSK: detail.encryptionType = "WPA2_PSK"; break;
      case WIFI_AUTH_WPA_WPA2_PSK: detail.encryptionType = "WPA_WPA2_PSK"; break;
      case WIFI_AUTH_WPA2_ENTERPRISE: detail.encryptionType = "WPA2_ENTERPRISE"; break;
      case WIFI_AUTH_WPA3_PSK: detail.encryptionType = "WPA3_PSK"; break;
      default: detail.encryptionType = "UNKNOWN"; break;
    }
    wifiList.push_back(detail);
  }

  showWifiScreen();
}

void WifiAnalyzerScreen::showWifiScreen()
{
  screenState = SCREEN_SCAN;
  std::vector<ListEntryItem> wifiEntries = {};
  for (const auto& wifi : wifiList) {
    wifiEntries.push_back({wifi.ssid.c_str(), wifi.bssid.c_str()});
  }

  setEntries(wifiEntries);
}

void WifiAnalyzerScreen::onEnter(const ListEntryItem& entry)
{
  if (screenState == SCREEN_SCAN) {
    screenState = SCREEN_INFO;
    for (const auto& wifi : wifiList) {
      if (wifi.bssid == entry.value.c_str()) {
        Template::renderHead("WiFi Info");
        setEntries({
        {"SSID", wifi.ssid.c_str()},
        {"BSSID", wifi.bssid.c_str()},
        {"RSSI", wifi.rssi.c_str()},
        {"Channel", String(wifi.channel).c_str()},
        {"Encryption", wifi.encryptionType.c_str()}
        });
        break;
      }
    }
  }
}

void WifiAnalyzerScreen::onBack()
{
  if (screenState == SCREEN_INFO) {
    showWifiScreen();
  } else {
    onEscape();
  }
}

void WifiAnalyzerScreen::onEscape()
{
  wifiList.clear();
  _global->setScreen(new WifiMenuScreen());
}
