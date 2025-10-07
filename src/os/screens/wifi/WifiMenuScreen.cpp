//
// Created by l-sha on 9/23/2025.
//

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"
#include "os/screens/wifi/WifiAnalyzerScreen.h"
#include "os/screens/wifi/WifiDeautherScreen.h"
#include "os/screens/wifi/WifiNetworkScreen.h"
#include "os/screens/wifi/WifiPacketMonitorScreen.h"

void WifiMenuScreen::onEnter(const ListEntryItem entry)
{
  if (entry.label == "Network")
  {
    _global->setScreen(new WifiNetworkScreen());
  } else if (entry.label == "WiFi Analyzer")
  {
    _global->setScreen(new WifiAnalyzerScreen());
  } else if (entry.label == "Packet Monitor")
  {
    _global->setScreen(new WifiPacketMonitorScreen());
  } else if (entry.label == "WiFi Deauther")
  {
    _global->setScreen(new WifiDeautherScreen());
  }
}
void WifiMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}