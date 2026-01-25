//
// Created by L Shaf on 17/12/25.
//

#pragma once

#include <WiFi.h>
#include "os/core/ListScreen.hpp"

class WifiAccessPointScreen final : public ListScreen
{
public:
  void init() override;

protected:
  bool isHidden = false;
  enum
  {
    STATE_MENU,
    STATE_QR_CODE
  } currentState = STATE_MENU;

  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;
  void onEscape() override;
  void preRender(M5Canvas& body) override;

  bool isAPCreated() const
  {
    return WiFi.getMode() == WIFI_MODE_AP || WiFi.getMode() == WIFI_MODE_APSTA;
  }

  int getYOffset() const override
  {
    if (isAPCreated()) return 10;
    return 0;
  };

  void renderMenu();
  void renderQRCode();
};
