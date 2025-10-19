//
// Created by L Shaf on 19/10/25.
//

#pragma once
#include "PN532Killer.h"
#include "os/core/ListScreen.hpp"

class NFCPN532Screen final : public ListScreen
{
public:
  void init() override;
  ~NFCPN532Screen() override;
protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;

private:
  PN532Killer _module = PN532Killer(Serial1);
  bool _isKiller = false;
  enum State_e
  {
    STATE_PN532,
    STATE_PN532_KILLER,
    STATE_SCAN_UID,
  } currentState = STATE_PN532;

  void goPN532Menu();
  void goPN532KillerMenu();
  void goActualMenu();

  void callScanUid();
};