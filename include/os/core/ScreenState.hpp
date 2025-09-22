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
public:
  ScreenState(): _global(&GlobalState::getInstance()) {};
};