//
// Created by l-sha on 9/23/2025.
//

#include "os/screens/wifi/WifiMenuScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/wifi/WifiAnalyzerScreen.h"
#include "os/screens/wifi/WifiNetworkScreen.h"

void WifiMenuScreen::onEnter(const ListEntryItem entry)
{
  if (entry.label == "Network")
  {
    _global->setScreen(new WifiNetworkScreen());
  } else if (entry.label == "WiFi Analyzer")
  {
    _global->setScreen(new WifiAnalyzerScreen());
  }
}
void WifiMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}