//
// Created by l-sha on 23/10/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class UtilityMenuScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
};