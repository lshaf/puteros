//
// Created by l-sha on 9/19/2025.
//
#pragma once

#include <vector>
#include <string>

#include "ScreenState.hpp"
#include "os/component/Template.hpp"

struct ListEntryItem {
  std::string label;
  std::string value;
};

class ListScreen : public ScreenState {
protected:
  std::vector<ListEntryItem> entries;
  int selectedIndex = 0;
  int previousIndex = -1;
  const int perPage = 10;
  int currentPage = 0;

  int scrollOffset = 0;
  int visibleCount = 0; // Set in constructor based on screen size

  enum NavAction_t {
    NAV_UP,
    NAV_DOWN,
    NAV_PREV,
    NAV_NEXT,
};

  virtual void onEnter(ListEntryItem entry) = 0;
  virtual void onEscape() { onBack(); };
  virtual void onBack() = 0;

  void navigate(NavAction_t direction);
public:
  ListScreen() = default;
  ~ListScreen() override { entries.clear(); };
  void setEntries(const std::vector<ListEntryItem>& newEntries);
  void update() override;
  void render() override;
};
