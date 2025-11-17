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
    "FreeWiFi_Lobby",
    "CoffeeShop_WiFi",
    "Cafe_Free_WiFi",
    "Library_Public",
    "CampusWiFi",
    "Student_WiFi",
    "Faculty_Net",
    "Guest_Access",
    "Lobby_WiFi",
    "IoT_Devices",
    "SmartHome",
    "Home_Network",
    "Home_5G",
    "Office_WiFi",
    "WorkNetwork",
    "CorpGuest",
    "Conference_WiFi",
    "Hotel_Guest",
    "Hotel_Free",
    "Airport_Free_WiFi",
    "Train_WiFi",
    "Bus_WiFi",
    "Shop_WiFi",
    "Mall_Guest",
    "Library_WiFi",
    "Studio_WiFi",
    "Garage_Network",
    "LivingRoom_WiFi",
    "Bedroom_WiFi",
    "Kitchen_AP",
    "SECURE-NET",
    "NETGEAR_Guest",
    "Linksys_Public",
    "TPLink_Hotspot",
    "XfinityWiFi",
    "SafeZone",
    "Neighbor_WiFi",
    "Free4All",
    "PublicHotspot",
    "FreeCityWiFi",
    "CommunityNet",
    "FreeTransitWiFi",
    "BlueSky_WiFi",
    "GreenCafe_WiFi",
    "Sunset_Hotspot"
  };

  WifiAttackUtility* attacker = nullptr;
};
