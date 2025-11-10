//
// Created by l-sha on 23/10/2025.
//

#pragma once

#include "os/core/ScreenState.hpp"

class UtilityI2CDetectorScreen final : public ScreenState
{
public:
  ~UtilityI2CDetectorScreen() override;
  static constexpr size_t I2C_ADDR_COUNT = 0x78;

  void init() override;
  void update() override;
  void render() override;

  int hextToDigit(char c);
};
