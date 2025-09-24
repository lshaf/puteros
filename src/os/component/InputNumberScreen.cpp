//
// Created by l-sha on 9/23/2025.
//

#include "os/component/InputNumberScreen.h"
#include "os/component/Template.hpp"

InputNumberScreen::InputNumberScreen(const std::string& title, const int initial)
    : title(title), buffer(std::to_string(initial))
{
  init();
}

void InputNumberScreen::init()
{
  Template::renderHead(title, true);
  render();
}

void InputNumberScreen::render()
{
  auto body = Template::createBody();
  body.fillSprite(BLACK);
  body.setTextColor(TFT_WHITE);
  body.setTextSize(2.5);
  body.drawCenterString(buffer.c_str(), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}

void InputNumberScreen::update() {
  const auto _keyboard = &M5Cardputer.Keyboard;

  if (_keyboard->isChange() && _keyboard->isPressed()) {
    const auto state = _keyboard->keysState();
    for (const auto i : state.word) {
      if (std::isdigit(i)) buffer.push_back(i); // Only append digits
    }
    if (state.del) { if (!buffer.empty()) buffer.pop_back(); }
    if (state.enter) done = true;

    // Validate the number within range
    if (!buffer.empty()) {
      int value = std::stoi(buffer);
      if (value < min_input) value = min_input;
      if (value > max_input) value = max_input;
      buffer = std::to_string(value);
    } else
    {
      buffer = std::to_string(min_input);
    }

    render();
  }
}

int InputNumberScreen::popup(const std::string& title, const int initial, const int min, const int max)
{
  const auto input = new InputNumberScreen(title, initial);
  input->setRange(min, max);
  while (!input->isDone())
  {
    M5Cardputer.update();
    input->update();
    delay(10);
  }
  return input->getResult();
}
