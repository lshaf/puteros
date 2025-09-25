//
// Created by l-sha on 9/23/2025.
//

#include "os/component/InputNumberScreen.h"
#include "os/component/Template.hpp"

InputNumberScreen::InputNumberScreen(const std::string& title, const int initial)
    : title(title), buffer(std::to_string(initial)) {}

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

  char buff[25];
  sprintf(buff, "%d - %d", min_input, max_input);
  body.setTextSize(1);
  body.drawCenterString(buff, body.width() / 2, body.height() / 2 - body.fontHeight() * 2 - 5);
  if (isInvalid)
  {
    body.setTextColor(TFT_RED);
    body.drawCenterString("Invalid Value", body.width() / 2, body.height() / 2 + body.fontHeight() + 2);
  }
  Template::renderBody(&body);
}

void InputNumberScreen::update() {
  const auto _keyboard = &M5Cardputer.Keyboard;

  if (_keyboard->isChange() && _keyboard->isPressed()) {
    isInvalid = false;
    const auto state = _keyboard->keysState();
    for (const auto i : state.word) {
      if (std::isdigit(i)) buffer.push_back(i);
    }
    if (state.del) { if (!buffer.empty()) buffer.pop_back(); }

    if (buffer.empty()) buffer = "0";
    unsigned int v = std::stoi(buffer);
    if (v >= cap_input) v = cap_input;
    if (v < min_input || v > max_input) isInvalid = true;
    buffer = std::to_string(v);

    if (state.enter && !isInvalid) done = true;

    render();
  }
}

int InputNumberScreen::popup(const std::string& title, const int initial, const unsigned int min, const unsigned int max)
{
  const auto input = new InputNumberScreen(title, initial);
  input->setRange(min, max);
  input->init();
  while (!input->isDone())
  {
    M5Cardputer.update();
    input->update();
    delay(10);
  }
  return input->getResult();
}
