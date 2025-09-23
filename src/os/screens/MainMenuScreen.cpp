//
// Created by l-sha on 9/20/2025.
//
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/WelcomeScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"

void MainMenuScreen::init()
{
  Template::renderHead("Main Menu");
  setEntries({"Wifi", "Bluetooth", "NFC", "Files", "Settings"});
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
    Template::renderHead("Main Menu");
    setEntries(MAIN_MENU);
    render();
  }
}

void MainMenuScreen::reworkMenu(const std::vector<MenuEntry>& menu)
{
  currentEntries = menu;
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
      _global->setScreen(new WifiMenuScreen());
    } else if (entry == "Bluetooth")
    {
      Template::renderHead(entry);
      currentState = STATE_BLUETOOTH;
      reworkMenu(BLUETOOTH_MENU);
    } else if (entry == "NFC")
    {
      Template::renderHead(entry);
      currentState = STATE_NFC;
      reworkMenu(NFC_MENU);
    } else if (entry == "Files")
    {
      _global->setScreen(new FileNavigatorScreen("/"));
    } else if (entry == "Settings")
    {
      Template::renderHead(entry);
      currentState = STATE_SETTINGS;
      reworkMenu(SETTINGS_MENU);
    }
  } else if (!currentEntries.empty())
  {
    for (const auto& item : currentEntries)
    {
      if (item.label == entry)
      {
        if (item.screen != nullptr)
        {
          _global->setScreen(item.screen);
        } else
        {
          // Placeholder for unimplemented features
          Template::renderHead(entry);
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