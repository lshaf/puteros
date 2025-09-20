//
// Created by l-sha on 9/20/2025.
//
#include "os/screens/wifi/WifiConnectScreen.h"

#include "os/core/ScreenManager.hpp"
#include "os/screens/MainMenuScreen.hpp"

WifiConnectScreen::WifiConnectScreen()
{
  //
}

void WifiConnectScreen::onBack()
{
  ScreenManager::to(new MainMenuScreen());
}

void WifiConnectScreen::onEnter(const std::string& entry)
{
  M5Cardputer.Speaker.tone(1000, 100);
}