//
// Created by l-sha on 9/23/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class SettingScreen final : public ListScreen
{
private:
  enum State_t {
    STATE_MAIN,
    STATE_ABOUT,
    STATE_PRIMARY_COLOR
  } currentState = STATE_MAIN;

  void renderAbout();
  void renderPrimaryColor();
  void refreshMenu(bool reset = false);
protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;

public:
  void init() override;
  void update() override;
};