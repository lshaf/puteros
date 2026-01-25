//
// Created by l-sha on 9/23/2025.
//

#include "os/screens/MainMenuScreen.hpp"
#include "WifiMenuScreen.h"

#include "WifiAnalyzerScreen.h"
#include "WifiBeaconSpamScreen.h"
#include "WifiDeauthDetectorScreen.h"
#include "WifiDeautherScreen.h"
#include "WifiESPNowChatScreen.h"
#include "WifiPacketMonitorScreen.h"

#include "network/WifiNetworkScreen.h"
#include "access_point/WifiAccessPointScreen.h"

void WifiMenuScreen::init()
{
  Template::renderHead("WiFi");
  setEntries({
    {"Network"},
    {"Access Point"},
    {"WiFi Analyzer"},
    {"Packet Monitor"},
    {"WiFi Deauther"},
    {"Deauther Detector"},
    {"Beacon Spam"},
    {"ESPNOW Chat"}
  });
}

void WifiMenuScreen::onEnter(const ListEntryItem& entry)
{
  if (entry.label == "Network")
  {
    _global->setScreen(new WifiNetworkScreen());
  } else if (entry.label == "Access Point")
  {
    _global->setScreen(new WifiAccessPointScreen());
  } else if (entry.label == "WiFi Analyzer")
  {
    _global->setScreen(new WifiAnalyzerScreen());
  } else if (entry.label == "Packet Monitor")
  {
    _global->setScreen(new WifiPacketMonitorScreen());
  } else if (entry.label == "WiFi Deauther")
  {
    _global->setScreen(new WifiDeautherScreen());
  } else if (entry.label == "Deauther Detector")
  {
    _global->setScreen(new WifiDeauthDetectorScreen());
  } else if (entry.label == "Beacon Spam")
  {
    _global->setScreen(new WifiBeaconSpamScreen());
  } else if (entry.label == "ESPNOW Chat")
  {
    _global->setScreen(new WifiESPNowChatScreen());
  }
}
void WifiMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}