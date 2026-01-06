//
// Created by L Shaf on 2025-12-29.
//

#pragma once

#include "os/core/ListScreen.hpp"
#include "TinyGPS++.h"
#include "os/utility/CircularArray.h"

class ModuleGPSScreen final : public ListScreen
{
public:
  void init() override;
  bool isAutoPowerOff() override
  {
    return currentState != STATE_WARDRIVER_ACTION;
  }

protected:
  int getYOffset() const override { return 16; }
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;
  void preRender(M5Canvas& body) override;
  void render() override;

  void renderInfoScreen();
  void renderMenuScreen();
  void renderWardriverScreen();

  void endService();

  enum
  {
    STATE_LOADING,
    STATE_MENU,
    STATE_INFO,
    STATE_WARDRIVER_ACTION
  } currentState = STATE_LOADING;

  std::string savePath = "/puteros/gps/wardriver";
  std::string filename;

  unsigned long lastRenderTime = 0;
  unsigned long lastUpdateTime = 0;
  TinyGPSPlus gps;

  bool isScanning = false;
  uint16_t totalDiscovered = 0;
  unsigned long lastWifiScan = 0;
  using BSSID = std::array<uint8_t, 6>;
  CircularBuffer<BSSID> scannedBSSIDs{50};

private:
  void addWigleRecord(
    const std::string& ssid, const std::string& bssid, const std::string& authMode,
    int32_t rssi, int32_t channel
  );

  String getCurrentGPSDate()
  {
    return String(gps.date.year()) + "-" + HelperUtility::padNumber(gps.date.month(), 2) + "-" + HelperUtility::padNumber(gps.date.day(), 2);
  }

  String getCurrentGPSTime()
  {
    return HelperUtility::padNumber(gps.time.hour(), 2) + ":" + HelperUtility::padNumber(gps.time.minute(), 2) + ":" + HelperUtility::padNumber(gps.time.second(), 2);
  }

  bool isBssidScanned(const uint8_t* bssid)
  {
    BSSID bssidArray;
    std::copy(bssid, bssid + 6, bssidArray.begin());
    for (int i = 0; i < scannedBSSIDs.size(); ++i)
    {
      const auto& scanned = scannedBSSIDs.get(i);
      if (scanned == bssidArray)
      {
        return true;
      }
    }
    scannedBSSIDs.push(bssidArray);
    return false;
  }
};
