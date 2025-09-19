// InputScreen.hpp
#pragma once
#include "Screen.hpp"
#include <string>

class InputScreen : public Screen {
public:
  InputScreen(const std::string& title, const std::string& initial = "");
  void update() override;
  bool isDone() const { return done; }
  const std::string& getResult() const { return buffer; }

  static std::string popup(const std::string& title, const std::string& initial = "");
private:
  std::string title;
  std::string buffer;
  bool done = false;
  bool showCursor = true;
  unsigned long lastBlink = 0;

  void updateBody();
};
