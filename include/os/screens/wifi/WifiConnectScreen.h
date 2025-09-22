//
// Created by l-sha on 9/20/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"

class WifiConnectScreen final : public ListScreen
{
private:
  enum
  {
    STATE_SELECT_WIFI,
    STATE_MENU,
    STATE_CLOCK
  } currentState = STATE_SELECT_WIFI;

  void showWifiList();
public:
  ~WifiConnectScreen() override;

  void init() override;
  void onBack() override;
  void update() override;
  void onEnter(const std::string& entry) override;
};
