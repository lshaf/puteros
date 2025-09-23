//
// Created by l-sha on 9/20/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"
#include "os/component/Template.hpp"

class MainMenuScreen final : public ListScreen
{
private:
  typedef struct
  {
    std::string label;
    Screen* screen;
  } MenuEntry;
  enum MenuState
  {
    STATE_MAIN,
    STATE_WIFI,
    STATE_BLUETOOTH,
    STATE_NFC,
    STATE_SETTINGS
  } currentState = STATE_MAIN;
  std::vector<MenuEntry> currentEntries{};
  std::vector<std::string> MAIN_MENU = {"Wifi", "Bluetooth", "NFC", "Files", "Settings"};
  std::vector<MenuEntry> BLUETOOTH_MENU = {
    {"Keyboard", nullptr},
    {"BadBLE", nullptr},
  };
  std::vector<MenuEntry> NFC_MENU = {
    {"M5 NFC", nullptr},
    {"PN532", nullptr},
  };
  std::vector<MenuEntry> SETTINGS_MENU = {
    {"Brightness", nullptr},
    {"About", nullptr},
  };

public:
  void init() override;
  void onBack() override;
  void onEnter(const std::string& entry) override;
  void reworkMenu(const std::vector<MenuEntry>& menu);
};
