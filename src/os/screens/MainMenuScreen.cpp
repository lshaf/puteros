//
// Created by l-sha on 9/20/2025.
//
#include "os/core/ScreenManager.hpp"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/WelcomeScreen.hpp"

MainMenuScreen::MainMenuScreen()
{
  setTitle("Main Menu");
  setEntries({"Wifi", "Bluetooth", "NFC", "Files", "Settings"});
  render();
}

void MainMenuScreen::onBack()
{
  if (currentState == STATE_MAIN)
  {
    ScreenManager::to(new WelcomeScreen());
  } else
  {
    currentState = STATE_MAIN;
    setTitle("Main Menu");
    setEntries(MAIN_MENU);
    render();
  }
}

void MainMenuScreen::reworkMenu(const std::vector<MenuEntry>& menu)
{
  std::vector<std::string> labels;
  for (const auto& item : menu)
  {
    labels.push_back(item.label);
  }
  setEntries(labels);
  render();
}

void MainMenuScreen::onEnter(const std::string& entry)
{
  if (currentState == STATE_MAIN)
  {
    if (entry == "Wifi")
    {
      setTitle(entry);
      currentState = STATE_WIFI;
      reworkMenu(WIFI_MENU);
    } else if (entry == "Bluetooth")
    {
      setTitle(entry);
      currentState = STATE_BLUETOOTH;
      reworkMenu(BLUETOOTH_MENU);
    } else if (entry == "NFC")
    {
      setTitle(entry);
      currentState = STATE_NFC;
      reworkMenu(NFC_MENU);
    } else if (entry == "Files")
    {
      ScreenManager::to(new FileNavigatorScreen("/"));
    } else if (entry == "Settings")
    {
      setTitle(entry);
      currentState = STATE_SETTINGS;
      reworkMenu(SETTINGS_MENU);
    }
  }
}