//
// Created by l-sha on 9/21/2025.
//
#pragma once

#include "core/Config.h"
#include "core/Screen.hpp"
#include <SD.h>

#define APP_CONFIG_BRIGHTNESS "brightness"
#define APP_CONFIG_BRIGHTNESS_DEFAULT "70"
#define APP_CONFIG_DEVICE_NAME "device_name"
#define APP_CONFIG_DEVICE_NAME_DEFAULT "Puter"
#define APP_CONFIG_VOLUME "volume"
#define APP_CONFIG_VOLUME_DEFAULT "75"
#define APP_CONFIG_NAV_SOUND "nav_sound"
#define APP_CONFIG_NAV_SOUND_DEFAULT "0"

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

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

  void loadSDCard()
  {
    if (!isSDCardInit)
    {
      SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);
      isSDCardLoaded = SD.begin(SD_SPI_CS_PIN, SPI, 25000000);
      isSDCardInit = true;
    }
  }

  bool getIsSDCardLoaded()
  {
    return isSDCardLoaded;
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
  bool isSDCardInit = false;
  bool isSDCardLoaded = false;
};
