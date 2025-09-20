//
// Created by L Shaf on 18/09/25.
//
#pragma once
#include "Screen.hpp"

class ScreenManager {
public:
  Screen* currentScreen = nullptr;
  static ScreenManager& getInstance() {
    static ScreenManager instance;
    return instance;
  }

  static void to(Screen* screen)
  {
    getInstance().setScreen(screen);
  }

  void setScreen(Screen* screen) {
    if (currentScreen) delete currentScreen;
    currentScreen = screen;
  }

  void update() const {
    if (currentScreen) currentScreen->update();
  }

  ~ScreenManager() {
    if (currentScreen) delete currentScreen;
  }
private:
  ScreenManager() = default;
  ScreenManager(const ScreenManager&) = delete;
  ScreenManager& operator=(const ScreenManager&) = delete;
};