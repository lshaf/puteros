//
// Created by L Shaf on 18/09/25.
//
#pragma once

#include "../Screen.hpp"
#include <M5Cardputer.h>
#include <vector>
#include <string>

class FileNavigatorScreen final : public Screen
{
  std::string currentPath = "/";
  std::vector<std::string> entries;
  int selectedIndex = 0;
  int previousIndex = 0;

  M5Canvas canvas;

  typedef enum
  {
    NAV_UP,
    NAV_DOWN,
    NAV_ENTER,
  } NavAction_t;

  void listDirectory(const std::string& path);
  void updateSelection();
  void navigate(NavAction_t direction);

public:
  explicit FileNavigatorScreen(std::string  path);
  void update() override;
  void render() override;
};