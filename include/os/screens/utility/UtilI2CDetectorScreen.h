//
// Created by l-sha on 23/10/2025.
//

#pragma once

#include "os/core/ScreenState.hpp"

class UtilI2CDetectorScreen final : public ScreenState
{
public:
  ~UtilI2CDetectorScreen() override;
  static constexpr size_t I2C_ADDR_COUNT = 0x78;

  void init() override;
  void update() override;
  void render() override;

  int hextToDigit(char c);
};
