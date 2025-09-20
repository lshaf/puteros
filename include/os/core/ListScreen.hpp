//
// Created by l-sha on 9/19/2025.
//
#pragma once
#include "Screen.hpp"
#include <vector>
#include <string>

class ListScreen : public Screen {
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

  void showList();
  void updateSelection();
  void navigate(NavAction_t direction);

public:
  ListScreen();
  void setEntries(const std::vector<std::string>& newEntries);
  void update() override;
};
