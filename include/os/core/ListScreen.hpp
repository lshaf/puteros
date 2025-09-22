//
// Created by l-sha on 9/19/2025.
//
#pragma once

#include <vector>
#include <string>

#include "ScreenState.hpp"
#include "os/component/Template.hpp"

class ListScreen : public ScreenState {
protected:
  std::vector<std::string> entries;
  int selectedIndex = 0;
  int previousIndex = -1;
  const int perPage = 10;
  int currentPage = 0;

  int scrollOffset = 0;
  int visibleCount = 0; // Set in constructor based on screen size

  M5Canvas selector;

  enum NavAction_t {
    NAV_UP,
    NAV_DOWN,
    NAV_PREV,
    NAV_NEXT,
    NAV_ENTER,
    NAV_BACK,
};

  virtual void onEnter(const std::string& entry) {}
  virtual void onBack() {}

  void navigate(NavAction_t direction, bool render = true);
public:
  ListScreen(): selector(&M5Cardputer.Lcd) {};
  ~ListScreen() override { entries.clear(); };
  void setEntries(const std::vector<std::string>& newEntries);
  void update() override;
  void init() override;
  void render() override;
};
