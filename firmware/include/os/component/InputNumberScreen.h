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
  void setRange(const unsigned int i_min, const unsigned int i_max)
  {
    min_input = min(i_min, cap_input);
    max_input = min(i_max, cap_input);
  }

  static int popup(const std::string& title, int initial = 0, unsigned int min = 0, unsigned int max = 100);
  void render() override;
  void init() override;
private:
  std::string title;
  unsigned int min_input = 0;
  unsigned int max_input = 100;
  unsigned int cap_input = 10000;
  std::string buffer;
  bool done = false;
  bool isInvalid = false;
};
