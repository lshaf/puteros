//
// Created by l-sha on 9/23/2025.
//
#pragma once
#include "os/core/ListScreen.hpp"

class WifiMenuScreen final : public ListScreen
{
public:
  void init() override;

  void onEnter(ListEntryItem entry) override;
  void onBack() override;
};
