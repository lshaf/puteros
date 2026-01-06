//
// Created by l-sha on 18/11/2025.
//

#pragma once
#include "esp_bt.h"

#include "NimBLEDevice.h"
#include "os/core/ScreenState.hpp"

class BLESpamScreen final : public ScreenState
{
private:
  BLEAdvertising* pAdvertising = nullptr;

protected:
  int counter = 0;
  const char* loadingBar = "-\\|/";
  unsigned long lastSendTime = 0;

  void stopBLE();
public:
  bool isAutoPowerOff() override {return false;}
  void init() override;
  void update() override;
  void render() override;
};
