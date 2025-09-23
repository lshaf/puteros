//
// Created by l-sha on 9/23/2025.
//
#pragma once

#include "../core/ScreenState.hpp"

class InputNumberScreen final : public ScreenState {
public:
  explicit InputNumberScreen(const std::string& title, int initial = 0);
  void update() override;
  bool isDone() const { return done; }
  int getResult() const { return std::stoi(buffer); }
  void setRange(const int min, const int max)
  {
    min_input = min;
    max_input = max;
  }

  static int popup(const std::string& title, int initial = 0, int min = 0, int max = 100);
  void render() override;
  void init() override;
private:
  std::string title;
  int min_input = 0;
  int max_input = 100;
  std::string buffer;
  bool done = false;
};
