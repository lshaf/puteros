//
// Created by l-sha on 9/20/2025.
//
#pragma once

#include "WelcomeScreen.hpp"
#include "os/core/ListScreen.hpp"

class MainMenuScreen final : public ListScreen
{
public:
  MainMenuScreen();
  void onBack() override;
  void onEnter(const std::string& entry) override;
};
