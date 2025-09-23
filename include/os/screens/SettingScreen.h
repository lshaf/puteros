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
    STATE_ABOUT
  } currentState = STATE_MAIN;

  void renderAbout();
protected:
  void onEnter(const std::string& entry) override;
  void onBack() override;

public:
  void init() override;
  void update() override;
};