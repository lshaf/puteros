//
// Created by l-sha on 9/20/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"

class WifiConnectScreen final : public ListScreen
{
public:
  WifiConnectScreen();
  void onBack() override;
  void onEnter(const std::string& entry) override;
};
