//
// Created by L Shaf on 18/09/25.
//
#pragma once

#pragma once

#include "../core/ListScreen.hpp"
#include <string>
#include <vector>

class FileNavigatorScreen final : public ListScreen {
  std::string currentPath = "/";

  void listDirectory(const std::string& path);

protected:
  void onEnter(const std::string& entry) override;
  void onBack() override;

public:
  explicit FileNavigatorScreen(const std::string& path);
  void update() override;
};
