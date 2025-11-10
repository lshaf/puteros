// InputScreen.cpp

#include "os/component/Template.hpp"
#include "os/component/InputTextScreen.hpp"
#include "M5Cardputer.h"

InputTextScreen::InputTextScreen(const std::string& title, const std::string& initial)
    : title(title), buffer(initial)
{
  init();
}

void InputTextScreen::init()
{
  Template::renderHead(title, true);
  Template::renderBody();
}

void InputTextScreen::render()
{
  auto body = Template::createBody();
  body.fillSprite(BLACK);
  body.setTextColor(TFT_WHITE);
  body.setTextSize(1);
  body.setCursor(0, 0);
  std::string display = buffer;
  if (showCursor) display += "_";
  body.print(display.c_str());
  Template::renderBody(&body);
}

void InputTextScreen::update() {
  const auto _keyboard = &M5Cardputer.Keyboard;
  const unsigned long now = millis();
  if (now - lastBlink >= 500) {
    showCursor = !showCursor;
    lastBlink = now;
    render();
  }

  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    const auto state = _keyboard->keysState();
    for (const auto i: state.word) buffer.push_back(i);
    if (state.del) { if (!buffer.empty()) buffer.pop_back(); }
    if (state.enter) done = true;

    render();
  }
}

std::string InputTextScreen::popup(const std::string& title, const std::string& initial)
{
  const auto input = new InputTextScreen(title, initial);
  while (!input->isDone())
  {
    M5Cardputer.update();
    input->update();
    delay(10);
  }
  return input->getResult();
}
