//
// Created by L Shaf on 18/09/25.
//
#pragma once

#pragma once

#include "../core/ListScreen.hpp"
#include <string>

class FileNavigatorScreen final : public ListScreen {
  std::string currentPath = "/";

  void listDirectory(const std::string& path);

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;

public:
  explicit FileNavigatorScreen(std::string  path);
  void update() override;
  void init() override;
};
