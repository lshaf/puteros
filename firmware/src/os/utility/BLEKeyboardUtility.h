//
// Created by l-sha on 03/11/2025.
//

#pragma once

#include "NimBLEDevice.h"
#include "NimBLEHIDDevice.h"
#include "HIDKeyboardUtility.h"

#define HID_OFFSET 0x88

class BLEKeyboardUtility final : public HIDKeyboardUtility, public NimBLEServerCallbacks, public NimBLECharacteristicCallbacks
{
private:
  NimBLEServer* bleServer;
  NimBLEHIDDevice* hid;
  NimBLECharacteristic* inputKeyboard;
  NimBLECharacteristic* outputKeyboard;
  NimBLECharacteristic* inputMedia;
  NimBLEAdvertising* advertising;

  std::string deviceName;
  uint8_t batteryLevel;
  bool connected = false;
  bool isInitialized = false;

public:
  ~BLEKeyboardUtility() override;
  explicit BLEKeyboardUtility(std::string deviceName = "ESP32 Keyboard", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
  void begin() override;
  void end() override;
  void sendReport(KeyReport* keys) override;

  void resetPair();

  bool isConnected();
  void setBatteryLevel(uint8_t level)
  {
    this->batteryLevel = level;
    if (hid) hid->setBatteryLevel(level);
  }
  void setName(std::string deviceName)
  {
    this->deviceName = std::move(deviceName);
  }


  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override;
};
