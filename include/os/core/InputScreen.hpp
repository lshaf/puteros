// InputScreen.hpp
#pragma once

#include <string>
#include "ScreenState.hpp"
#include "os/component/Template.hpp"

class InputScreen final : public ScreenState {
public:
  explicit InputScreen(const std::string& title, const std::string& initial = "");
  void update() override;
  bool isDone() const { return done; }
  const std::string& getResult() const { return buffer; }

  static std::string popup(const std::string& title, const std::string& initial = "");
  void render() override;
  void init() override;
private:
  std::string title;
  std::string buffer;
  bool done = false;
  bool showCursor = true;
  unsigned long lastBlink = 0;
};
