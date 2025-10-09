//
// Created by l-sha on 09/10/2025.
//

#pragma once
#include <NimBLEDevice.h>
#include "os/core/ListScreen.hpp"

class BLEAnalyzerScreen final : public ListScreen
{
public:
  ~BLEAnalyzerScreen() override
  {
    stopScan();
  };
  void init() override;
  void startScan();
  void showResult();
  void showInfo();
  void stopScan()
  {
    if (bleScan != nullptr)
    {
      bleScan->stop();
      NimBLEDevice::deinit(true);
      bleScan = nullptr;
    }
    if (selectedDevice != nullptr)
    {
      selectedDevice = nullptr;
    }
  }

protected:
  void onEnter(ListEntryItem entry) override;
  void onEscape() override;
  void onBack() override;

private:
  enum State_e
  {
    STATE_SCAN,
    STATE_LIST,
    STATE_SERVICE_UUID,
    STATE_SERVICE_DATA,
    STATE_MANUFACTURE_DATA,
    STATE_INFO
  } currentState = STATE_SCAN;

  NimBLEScan* bleScan = nullptr;
  NimBLEScanResults scanResults;
  const NimBLEAdvertisedDevice* selectedDevice = nullptr;
};
