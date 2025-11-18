//
// Created by l-sha on 25/10/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class GameMenuScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
};
