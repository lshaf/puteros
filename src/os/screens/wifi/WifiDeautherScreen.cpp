//
// Created by l-sha on 10/6/2025.
//

#include "os/screens/wifi/WifiDeautherScreen.h"
#include "os/screens/wifi/WifiMenuScreen.h"

WifiDeautherScreen::~WifiDeautherScreen()
{
  if (attacker != nullptr)
  {
    delete attacker;
    attacker = nullptr;
  }
}

void WifiDeautherScreen::init()
{
  Template::renderHead("WiFi Deauther");
  currentState = STATE_MAIN;
  setEntries({
    {"Target Wifi", target.ssid.c_str()},
    {"Start Deauth"},
  });
}

void WifiDeautherScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MAIN)
  {
    if (entry.label == "Start Deauth")
    {
      if (target.ssid == "-" && memcmp(target.bssid, blankMac, 6) == 0)
      {
        Template::drawStatusBody("Select a target first!");
        delay(2);
        init();
        return;
      }

      Template::drawStatusBody(("Deauthing " + target.ssid + "...").c_str());
      currentState = STATE_DEAUTHING;
      attacker = new WifiAttackUtility();
    } else if (entry.label == "Target Wifi")
    {
      currentState = STATE_SELECT_WIFI;
      Template::renderHead("Select WiFi", true);
      Template::drawStatusBody("Scanning...");
      std::vector<ListEntryItem> wifiList = {};
      WiFi.mode(WIFI_STA);
      const int totalWifi = WiFi.scanNetworks();
      for (int i = 0; i < totalWifi; i++)
      {
        char buffer[51];
        sprintf(buffer, "[%2d] %s", WiFi.channel(i), WiFi.SSID(i).c_str());
        wifiList.push_back({
          buffer,
          WiFi.BSSIDstr(i).c_str()
        });
      }

      setEntries(wifiList);
    }
  } else if (currentState == STATE_SELECT_WIFI)
  {
    target.ssid = entry.label;
    sscanf(entry.value.c_str(), "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
           &target.bssid[0], &target.bssid[1], &target.bssid[2],
           &target.bssid[3], &target.bssid[4], &target.bssid[5]);
    target.channel = atoi(entry.label.substr(1, 3).c_str());
    init();
  }
}


void WifiDeautherScreen::onBack()
{
  if (currentState == STATE_MAIN)
  {
    _global->setScreen(new WifiMenuScreen());
  } else if (currentState == STATE_SELECT_WIFI)
  {
    init();
  }
}

void WifiDeautherScreen::onEscape()
{
  _global->setScreen(new WifiMenuScreen());
}

void WifiDeautherScreen::update()
{
  if (currentState == STATE_MAIN || currentState == STATE_SELECT_WIFI)
  {
    ListScreen::update();
  } else if (currentState == STATE_DEAUTHING)
  {
    const auto _keyboard = &M5Cardputer.Keyboard;
    if (_keyboard->isChange() && _keyboard->isPressed())
    {
      if (_keyboard->isKeyPressed('`') || _keyboard->isKeyPressed(KEY_BACKSPACE))
      {
        delete attacker;
        attacker = nullptr;
        Template::drawStatusBody("Deauth stopped. Returning to menu...");
        delay(1000);
        init();
        return;
      }
    }

    if (attacker != nullptr)
    {
      attacker->deauthenticate(target.bssid, target.channel);
      // Show a simple animation or status update for user feedback
      static int dotCount = 0;
      Template::drawStatusBody(("[" + std::string(1, loadingBar[dotCount]) + "] Deauthing " + target.ssid + "...").c_str());
      dotCount = (dotCount + 1) % 4;
      delay(100);
    }
  }
}