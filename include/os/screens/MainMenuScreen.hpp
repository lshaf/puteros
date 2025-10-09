//
// Created by l-sha on 9/20/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"

class MainMenuScreen final : public ListScreen
{
public:
  void init() override
  {
    Template::renderHead("Main Menu");
    setEntries({
      {"Wifi"},
      {"Bluetooth"},
      {"NFC"},
      {"Files"},
      {"Settings"}
    });
  }

protected:
  void onBack() override;
  void onEnter(ListEntryItem entry) override;
};
