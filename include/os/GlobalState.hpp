//
// Created by l-sha on 9/21/2025.
//
#pragma once

#include <M5Cardputer.h>
#include "core/Screen.hpp"

class GlobalState {
public:
  GlobalState() = default;
  GlobalState(const GlobalState&) = delete;
  GlobalState& operator=(const GlobalState&) = delete;

  static GlobalState& getInstance()
  {
    static GlobalState instance;
    return instance;
  }

  void setScreen(Screen* screen)
  {
    if (currentScreen) delete currentScreen;
    currentScreen = screen;

    if (currentScreen != nullptr)
    {
      currentScreen->init();
      currentScreen->render();
    }
  }

  Screen* getScreen() const
  {
    return currentScreen;
  }

private:
  Screen* currentScreen = nullptr;
};
