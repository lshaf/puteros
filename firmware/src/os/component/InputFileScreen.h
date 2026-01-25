//
// Created by l-sha on 31/10/2025.
//
#pragma once

#include "os/core/ListScreen.hpp"

class InputFileScreen final : public ListScreen
{
public:
  explicit InputFileScreen(
    const std::string& title,
    const std::string& initial = "/"
  ) : title(title), initPath(initial), currentPath(initial)
  {
    init();
  }

  void init() override;
  bool isDone() const {return done;}
  const std::string& getResult() const {return selectedFile;}

  static std::string popup(
    const std::string& title,
    const std::string& initial = "/"
  );

protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;
  void onEscape() override;

private:
  std::string title;
  std::string initPath;
  std::string currentPath;
  std::string selectedFile;
  bool done = false;
};