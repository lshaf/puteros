//
// Created by l-sha on 9/26/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class WifiAnalyzerScreen final : public ListScreen
{
private:
  enum State_e
  {
    SCREEN_SCAN,
    SCREEN_INFO
  } screenState = SCREEN_SCAN;

  struct WifiDetail_t {
    String ssid;
    String bssid;
    String rssi;
    int32_t channel;
    String encryptionType;
  };

  std::vector<WifiDetail_t> wifiList;

  void grabWifiList();
  void showWifiScreen();
public:
  void init() override;

protected:
  void onEnter(const ListEntryItem& entry) override;
  void onEscape() override;
  void onBack() override;
};