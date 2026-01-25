//
// Created by L Shaf on 2026-01-16.
//

#pragma once
#include "os/core/ListScreen.hpp"
#include "os/utility/module/SIMModule.h"

class ModuleCATMScreen final : public ListScreen
{
public:
  void init() override;
  bool isKeepScreenOn() override {return true;}

protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;

private:
  SIMModule _gsm;
};
