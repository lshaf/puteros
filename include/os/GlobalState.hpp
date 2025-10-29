//
// Created by l-sha on 9/21/2025.
//
#pragma once

#include "core/Config.h"
#include "core/Screen.hpp"

#define CONFIG_BRIGHTNESS "brightness"
#define CONFIG_DEVICE_NAME "device_name"

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

  Config* getConfig()
  {
    if (config == nullptr)
    {
      config = new Config("/puteros/config");
    }

    return config;
  }
private:
  Screen* currentScreen = nullptr;
  Config* config = nullptr;
};
