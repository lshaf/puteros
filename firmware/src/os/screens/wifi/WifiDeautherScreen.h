//
// Created by l-sha on 10/6/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"
#include "os/utility/WifiAttackUtility.h"

class WifiDeautherScreen final: public ListScreen
{
public:
  ~WifiDeautherScreen() override;
  void init() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;

private:
  MacAddr blankMac = {0,0,0,0,0,0};
  const char* loadingBar = "-\\|/";
  struct Target_s
  {
    std::string ssid = "-";
    MacAddr bssid = {0,0,0,0,0,0};
    int channel = 1;
  } target;

  enum State_e
  {
    STATE_MAIN,
    STATE_SELECT_WIFI,
    STATE_DEAUTHING
  } currentState = STATE_MAIN;

  WifiAttackUtility* attacker = nullptr;
};