//
// Created by l-sha on 08/12/2025.
//
#pragma once

#include <WebServer.h>
#include "os/core/ListScreen.hpp"

class WiNetFileManager final : public ListScreen
{
public:
  void init() override;

protected:
  WebServer server{80};

  enum State_e
  {
    STATE_MENU,
    STATE_WEB_MANAGER
  } currentState = STATE_MENU;

  void onEnter(ListEntryItem entry) override;
  void onBack() override;
};
