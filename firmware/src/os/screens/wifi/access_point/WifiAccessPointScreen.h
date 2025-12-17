//
// Created by L Shaf on 17/12/25.
//

#pragma once
#include "os/core/ListScreen.hpp"

class WifiAccessPointScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;

  void renderMenu();
};
