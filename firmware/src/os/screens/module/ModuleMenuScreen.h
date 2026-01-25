//
// Created by l-sha on 09/10/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class ModuleMenuScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;
};