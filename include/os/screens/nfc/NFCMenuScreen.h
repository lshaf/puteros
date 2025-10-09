//
// Created by l-sha on 09/10/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class NFCMenuScreen final : public ListScreen
{
public:
  void init() override
  {
    Template::renderHead("NFC");
    Template::drawStatusBody("In Development");
  }

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
};