//
// Created by l-sha on 9/23/2025.
//
#pragma once
#include "os/core/ScreenState.hpp"

class WifiConnectClockScreen final : public ScreenState
{
private:
  int offsetMinutes = 0;
  unsigned long lastRenderTime = 0;
  bool timeToRefresh() const
  {
    return millis() - lastRenderTime > 500;
  };
public:
  void render() override;
  void init() override;
  void update() override;
};
