//
// Created by l-sha on 9/23/2025.
//

#include "os/screens/wifi/WifiMenuScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/wifi/WifiNetworkScreen.h"

void WifiMenuScreen::onEnter(const std::string& entry)
{
  if (entry == "Network")
  {
    _global->setScreen(new WifiNetworkScreen());
  }
}
void WifiMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}