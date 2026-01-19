//
// Created by l-sha on 9/20/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"

class WifiNetworkScreen final : public ListScreen
{
private:
  enum
  {
    STATE_SELECT_WIFI,
    STATE_MENU,
    STATE_INFORMATION,
    STATE_QR_WIFI
  } currentState = STATE_SELECT_WIFI;

  String passwordPath = "/m5geek/wifi/passwords";

  void showWifiList();
  void showMenu();
  void showQRCode();
  void showInformation();

  String passwordWifiPath(const String& bssid, const String& ssid);
  bool connectToWifi(const String& bssid, const String& ssid, const String& password);
public:
  void init() override;
  void onBack() override;
  void update() override;
  void onEnter(ListEntryItem entry) override;
};
