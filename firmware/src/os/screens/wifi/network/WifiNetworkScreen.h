//
// Created by l-sha on 9/20/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"

class WifiNetworkScreen final : public ListScreen
{
private:
  enum
  {
    STATE_SELECT_WIFI,
    STATE_MENU
  } currentState = STATE_SELECT_WIFI;

  void showWifiList();
  void showMenu();
public:
  void init() override;
  void onBack() override;
  void update() override;
  void onEnter(ListEntryItem entry) override;
};
