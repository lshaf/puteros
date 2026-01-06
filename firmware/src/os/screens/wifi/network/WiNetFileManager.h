//
// Created by l-sha on 08/12/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"
#include "os/utility/website/WebFileManager.h"

class WiNetFileManager final : public ListScreen
{
public:
  bool isAutoPowerOff() override {return currentState != STATE_WEB_MANAGER;}
  void init() override;

protected:
  WebFileManager server{};
  String currentPassword;

  enum State_e
  {
    STATE_MENU,
    STATE_WEB_MANAGER
  } currentState = STATE_MENU;

  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void update() override;

  void renderMainMenu();
};
