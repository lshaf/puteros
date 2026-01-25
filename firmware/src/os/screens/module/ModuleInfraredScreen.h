//
// Created by l-sha on 22/01/2026.
//

#pragma once
#include "os/core/ListScreen.hpp"

class ModuleInfraredScreen final : public ListScreen
{
protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;

  int getYOffset() const override {return 0;}
  void preRender(M5Canvas& body) override;

public:
  void init() override;
  void update() override;
};
