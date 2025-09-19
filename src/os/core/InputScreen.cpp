// InputScreen.cpp
#include "os/core/InputScreen.hpp"
#include "M5Cardputer.h"

InputScreen::InputScreen(const std::string& title, const std::string& initial)
    : title(title), buffer(initial)
{
  _header.fillSprite(TFT_BLACK);
  _header.setTextColor(TFT_WHITE);
  _header.setTextSize(1.5);
  _header.drawString(title.c_str(), 2, _header.height() / 2 - _header.fontHeight() / 2);
  render();
}

void InputScreen::updateBody() {
  _body.fillSprite(BLACK);
  _body.setTextColor(TFT_WHITE);
  _body.setTextSize(1);
  _body.setCursor(0, 0);
  std::string display = buffer;
  if (showCursor) display += "_";
  _body.print(display.c_str());
  render();
}

void InputScreen::update() {
  const unsigned long now = millis();
  if (now - lastBlink >= 500) {
    showCursor = !showCursor;
    lastBlink = now;
    updateBody();
  }

  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    const auto state = _keyboard->keysState();
    for (auto i: state.word) buffer.push_back(i);
    if (state.del) { if (!buffer.empty()) buffer.pop_back(); }
    if (state.enter) done = true;

    Serial.print("Debug Key: ");
    Serial.print("M" + String(state.modifiers) + "//");
    for (auto i: state.hid_keys) Serial.print(String(i) + "//");
    for (auto i: state.modifier_keys) Serial.print("[" + String(i) + "]");
    for (auto i: state.word) Serial.print(String(i));
    Serial.println();
    updateBody();
  }
}

std::string InputScreen::popup(const std::string& title, const std::string& initial)
{
  const auto input = new InputScreen(title, initial);
  while (!input->isDone())
  {
    M5Cardputer.update();
    input->update();
    delay(10);
  }
  return input->getResult();
}