//
// Created by l-sha on 17/11/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class KeyboardMenuScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;

private:
  bool isUSBMode = true;

  void refreshMenu(bool reset = true);
};