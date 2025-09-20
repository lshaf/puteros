//
// Created by l-sha on 9/20/2025.
//
#include "os/screens/MainMenuScreen.hpp"

#include "os/core/ScreenManager.hpp"
#include "os/screens/FileNavigatorScreen.hpp"

MainMenuScreen::MainMenuScreen()
{
  setTitle("Main Menu");
  setEntries({"Wifi", "Bluetooth", "NFC", "Files", "Settings"});
  render();
}

void MainMenuScreen::onBack()
{
  ScreenManager::to(new WelcomeScreen());
}

void MainMenuScreen::onEnter(const std::string& entry)
{
  if (entry == "Wifi")
  {
    M5Cardputer.Speaker.tone(7000, 1000);
  } else if (entry == "Bluetooth")
  {
    M5Cardputer.Speaker.tone(6000, 1000);
  } else if (entry == "NFC")
  {
    M5Cardputer.Speaker.tone(5000, 1000);
  } else if (entry == "Files")
  {
    ScreenManager::to(new FileNavigatorScreen("/"));
  } else if (entry == "Settings")
  {
    M5Cardputer.Speaker.tone(3000, 1000);
  }
}