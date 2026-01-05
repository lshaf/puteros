//
// Created by l-sha on 9/18/2025.
//
#pragma once

#include "os/core/ScreenState.hpp"

class WelcomeScreen final : public ScreenState
{
private:
  std::string _title = "PuterOS";
  bool _withBattery = true;
public:
  WelcomeScreen() = default;
  void update() override;
  void render() override;
};
