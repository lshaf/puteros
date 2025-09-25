//
// Created by l-sha on 9/20/2025.
//
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/SettingScreen.h"
#include "os/screens/WelcomeScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"

void MainMenuScreen::init()
{
  Template::renderHead("Main Menu");
  setEntries(MAIN_MENU);
}

void MainMenuScreen::onBack()
{
  if (currentState == STATE_MAIN)
  {
    _global->setScreen(new WelcomeScreen());
  } else
  {
    currentState = STATE_MAIN;
    currentEntries.clear();
    init();
  }
}

void MainMenuScreen::reworkMenu(const std::vector<MenuEntry>& menu)
{
  currentEntries = menu;
  std::vector<ListEntryItem> labels;
  for (const auto& item : menu)
  {
    labels.push_back({item.label, ""});
  }
  setEntries(labels);
}

void MainMenuScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MAIN)
  {
    if (entry.label == "Wifi")
    {
      _global->setScreen(new WifiMenuScreen());
    } else if (entry.label == "Bluetooth")
    {
      Template::renderHead(entry.label);
      currentState = STATE_BLUETOOTH;
      reworkMenu(BLUETOOTH_MENU);
    } else if (entry.label == "NFC")
    {
      Template::renderHead(entry.label);
      currentState = STATE_NFC;
      reworkMenu(NFC_MENU);
    } else if (entry.label == "Files")
    {
      _global->setScreen(new FileNavigatorScreen("/"));
    } else if (entry.label == "Settings")
    {
      _global->setScreen(new SettingScreen());
    }
  } else if (!currentEntries.empty())
  {
    for (const auto& item : currentEntries)
    {
      if (item.label == entry.label)
      {
        if (item.screen != nullptr)
        {
          _global->setScreen(item.screen);
        } else
        {
          // Placeholder for unimplemented features
          Template::renderHead(entry.label);
          auto body = Template::createBody();
          body.setTextColor(TFT_WHITE);
          body.drawCenterString("Not Implemented Yet", body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
          Template::renderBody(&body);
        }
        break;
      }
    }
  }
}