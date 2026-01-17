//
// Created by l-sha on 9/21/2025.
//
#pragma once

#include "M5GFX.h"
#include "core/Config.h"
#include "core/Screen.hpp"

#include <SD.h>
#include <ESPmDNS.h>
#include <ESPAsyncWebServer.h>

// Wifi File Manager Password
#define APP_CONFIG_WIFI_WEB_PASSWORD "wifi_web_password"
#define APP_CONFIG_WIFI_WEB_PASSWORD_DEFAULT "puteros123"

// Wifi Access Point
#define APP_CONFIG_WIFI_AP_SSID "wifi_ap_ssid"
#define APP_CONFIG_WIFI_AP_SSID_DEFAULT "PuterOS"
#define APP_CONFIG_WIFI_AP_PASSWORD "wifi_ap_password"
#define APP_CONFIG_WIFI_AP_PASSWORD_DEFAULT "puteros123"

// Device Settings
#define APP_CONFIG_PRIMARY_COLOR "primary_color"
#define APP_CONFIG_PRIMARY_COLOR_DEFAULT "Blue"
#define APP_CONFIG_BRIGHTNESS "brightness"
#define APP_CONFIG_BRIGHTNESS_DEFAULT "70"
#define APP_CONFIG_DEVICE_NAME "device_name"
#define APP_CONFIG_DEVICE_NAME_DEFAULT "Puter"
#define APP_CONFIG_VOLUME "volume"
#define APP_CONFIG_VOLUME_DEFAULT "75"
#define APP_CONFIG_NAV_SOUND "nav_sound"
#define APP_CONFIG_NAV_SOUND_DEFAULT "0"
#define APP_CONFIG_ENABLE_POWER_SAVING "enable_power_saving"
#define APP_CONFIG_ENABLE_POWER_SAVING_DEFAULT "1"
#define APP_CONFIG_INTERVAL_DISPLAY_OFF "interval_display_off"
#define APP_CONFIG_INTERVAL_DISPLAY_OFF_DEFAULT "10"
#define APP_CONFIG_INTERVAL_POWER_OFF "interval_power_off"
#define APP_CONFIG_INTERVAL_POWER_OFF_DEFAULT "60"

#define LORA_SPI_SCK_PIN  40
#define LORA_SPI_MISO_PIN 39
#define LORA_SPI_MOSI_PIN 14
#define LORA_SPI_CS_PIN   5

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

  void init()
  {
    pinMode(SD_SPI_CS_PIN, OUTPUT);
    pinMode(LORA_SPI_CS_PIN, OUTPUT);
    useSDCard();
    loadSDCard();
  }

  void useLora()
  {
    digitalWrite(SD_SPI_CS_PIN, HIGH);
    digitalWrite(LORA_SPI_CS_PIN, LOW);
  }

  void useSDCard()
  {
    digitalWrite(SD_SPI_CS_PIN, LOW);
    digitalWrite(LORA_SPI_CS_PIN, HIGH);
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

  bool getIsSDCardLoaded() const
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
    }
  }

  int getMainColor()
  {
    const String color = getConfig()->get(APP_CONFIG_PRIMARY_COLOR, APP_CONFIG_PRIMARY_COLOR_DEFAULT);
    if (color == "Red") return TFT_RED;
    if (color == "Green") return TFT_DARKGREEN;
    if (color == "Brown") return TFT_BROWN;
    if (color == "Cyan") return TFT_DARKCYAN;
    if (color == "Purple") return TFT_PURPLE;
    if (color == "Orange") return TFT_ORANGE;
    if (color == "Violet") return TFT_VIOLET;
    return TFT_BLUE;
  }

  AsyncWebServer* getServer()
  {
    if (server == nullptr)
    {
      if (MDNS.begin("puteros"))
        MDNS.addService("http", "tcp", 80);

      server = new AsyncWebServer(80);
      server->begin();
    }

    return server;
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
  AsyncWebServer* server = nullptr;

  bool isSDCardInit = false;
  bool isSDCardLoaded = false;
};
