//
// Created by l-sha on 09/10/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class BLEMenuScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
};