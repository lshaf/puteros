//
// Created by l-sha on 27/10/2025.
//

#pragma once

#include <vector>
#include "os/core/ScreenState.hpp"
#include "os/utility/WifiAttackUtility.h"

class WifiBeaconSpamScreen final : public ScreenState
{
public:
  ~WifiBeaconSpamScreen() override;

  void init() override;
  void update() override;
  void render() override;

protected:
  void broadcastWifiBeacon();

private:
  unsigned long lastRender = 0;
  uint8_t currentChannel = 1;
  const char* loadingBar = "-\\|/";
  std::vector<std::string> ssidList = {
    "FreeWiFi",
    "Free_WiFi",
    "FreeWifi_Hotspot",
    "Free Internet",
    "FreeWiFi_Cafe",
    "FreeWifi_Lounge",
    "Open_WiFi",
    "Open_Network",
    "Public_WiFi",
    "Guest_WiFi",
    "FreeHotspot",
    "FreeNet",
    "FreeConnect",
    "FreeAccess",
    "ComplimentaryWiFi",
    "FreeWiFiZone",
    "Free_WiFi_Hub",
    "FreeSignal",
    "FreeWiFi24_7",
    "Community_WiFi",
    "Free_WiFi_NearMe",
    "FreeWave",
    "FreeLink",
    "FreeRouter",
    "FreeZone_WiFi",
    "FreeCloud_WiFi",
    "FreeSpot",
    "FreeWave_Hotspot",
    "FreeNet_Public",
    "FreeWiFi_Lobby"
  };

  WifiAttackUtility* attacker = nullptr;
};
