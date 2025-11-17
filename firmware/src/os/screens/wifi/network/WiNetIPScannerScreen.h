//
// Created by l-sha on 9/24/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class WiNetIPScannerScreen final : public ListScreen
{
private:
  enum State_e
  {
    STATE_CONFIGURATION,
    STATE_SCANNING_IP,
    STATE_SCANNING_PORT,
    STATE_RESULT_IP,
    STATE_RESULT_PORT
  } currentState = STATE_CONFIGURATION;

  std::vector<ListEntryItem> ipList = {};
  int startIp = 1;
  int endIp = 254;

  void scanIP();
  void scanPort(const std::string& ip);
  void configuration();
protected:
  void init() override;
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
};
