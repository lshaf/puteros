//
// Created by l-sha on 9/21/2025.
//
#pragma once

#include "os/GlobalState.hpp"
#include "Screen.hpp"

class ScreenState: public Screen
{
protected:
  GlobalState* _global;
  Config* _config;
public:
  ScreenState(): _global(&GlobalState::getInstance()), _config(_global->getConfig()) {};
};