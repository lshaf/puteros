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
  std::string generateRandomSSID(uint8_t length);

private:
  unsigned long lastRender = 0;
  uint8_t loading = 0;
  uint8_t currentChannel = 1;
  std::vector<std::string> ssidList = {};

  WifiAttackUtility* attacker = nullptr;
};
