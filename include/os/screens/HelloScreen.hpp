//
// Created by l-sha on 9/18/2025.
//
#pragma once

#include "../core/Screen.hpp"
#include "FileNavigatorScreen.hpp"

class HelloScreen final : public Screen
{
public:
  HelloScreen() {
    _header.setTextColor(TFT_WHITE);
    _header.setTextSize(1.5);
    _header.drawString("PuterOS", 2, _header.height() / 2 - _header.fontHeight() / 2);

    _body.setTextColor(TFT_WHITE);
    _body.drawCenterString("Press ENTER to Start", _body.width() / 2, _body.height() / 2 - _body.fontHeight() / 2);

    render();
  }

  void update() override {
    if (_keyboard->isPressed() && _keyboard->isChange())
    {
      if (_keyboard->isKeyPressed(KEY_ENTER))
      {
        ScreenManager::getInstance().setScreen(new FileNavigatorScreen("/"));
      }
    }
  }
};