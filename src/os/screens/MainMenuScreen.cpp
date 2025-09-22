//
// Created by l-sha on 9/20/2025.
//
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/WelcomeScreen.hpp"

void MainMenuScreen::init()
{
  ListScreen::init();
  Template::renderHead("Main Menu");
  setEntries({"Wifi", "Bluetooth", "NFC", "Files", "Settings"});
  Serial.println("MainMenuScreen::init()");
}

void MainMenuScreen::onBack()
{
  if (currentState == STATE_MAIN)
  {
    _global->setScreen(new WelcomeScreen());
  } else
  {
    currentState = STATE_MAIN;
    Template::renderHead("Main Menu");
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
      Template::renderHead(entry);
      currentState = STATE_WIFI;
      reworkMenu(WIFI_MENU);
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
  }
}