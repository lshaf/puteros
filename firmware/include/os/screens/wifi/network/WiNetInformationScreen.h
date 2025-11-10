//
// Created by l-sha on 9/23/2025.
//
#pragma once

#include "os/core/ScreenState.hpp"

class WiNetInformationScreen final : public ScreenState
{
public:
  void init() override;
  void update() override;
  void render() override;
};