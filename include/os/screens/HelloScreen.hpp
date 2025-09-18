//
// Created by l-sha on 9/18/2025.
//
#pragma once

#include "../Screen.hpp"

class HelloScreen final : public Screen
{
public:
  HelloScreen() {
    _header.fillSprite(TFT_LIGHTGRAY);
    _header.setTextColor(TFT_WHITE);
    _header.drawString("Hello Screen", 2, 4);

    _body.fillSprite(TFT_LIGHTGRAY);
    _body.setTextColor(TFT_WHITE);
    _body.drawString("Welcome to the Hello Screen!", 0, 0);

    render();
  }

  void update() override {
    // No dynamic content to update in this simple screen
  }
};