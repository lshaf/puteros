//
// Created by L Shaf on 18/09/25.
//
#pragma once
#include "Screen.hpp"

class ScreenManager {
public:
  Screen* currentScreen = nullptr;

  void setScreen(Screen* screen) {
    if (currentScreen) delete currentScreen;
    currentScreen = screen;
  }

  void update() {
    if (currentScreen) currentScreen->update();
  }

  void render() {
    if (currentScreen && currentScreen->needsRefresh())
    {
      currentScreen->render();
      currentScreen->refresh(false);
    }
  }

  ~ScreenManager() {
    if (currentScreen) delete currentScreen;
  }
};