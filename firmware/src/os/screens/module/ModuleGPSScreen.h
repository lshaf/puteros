//
// Created by L Shaf on 2025-12-29.
//

#pragma once

#include "os/core/ListScreen.hpp"
#include "TinyGPS++.h"

class ModuleGPSScreen final : public ListScreen
{
public:
  void init() override;

protected:
  int getYOffset() const override { return 16; }
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void update() override;
  void preRender(M5Canvas& body) override;
  void render() override;

  void renderInfoScreen();
  void renderMenuScreen();
  void renderWardriverScreen();

  void endService();

  enum
  {
    STATE_LOADING,
    STATE_MENU,
    STATE_INFO,
    STATE_WARDRIVER_MENU,
    STATE_WARDRIVER_ACTION
  } currentState = STATE_LOADING;

  

  unsigned long lastRenderTime = 0;
  unsigned long lastUpdateTime = 0;
  TinyGPSPlus gps;
};
