//
// Created by L Shaf on 18/09/25.
//
#pragma once
#include "Screen.hpp"

class ScreenManager {
public:
  Screen* currentScreen = nullptr;

  void setScreen(Screen* screen) {
    Serial.println("ScreenManager::setScreen()");
    if (currentScreen) delete currentScreen;
    currentScreen = screen;
  }

  void update() const {
    if (currentScreen) currentScreen->update();
  }

  ~ScreenManager() {
    if (currentScreen) delete currentScreen;
  }
};