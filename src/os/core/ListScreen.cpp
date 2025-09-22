//
// Created by l-sha on 9/19/2025.
//
#include "os/core/ListScreen.hpp"

void ListScreen::init()
{
}

void ListScreen::setEntries(const std::vector<std::string>& newEntries)
{
  entries = newEntries;
  currentPage = 0;
  selectedIndex = 0;
  previousIndex = -1;
  scrollOffset = 0;
  render();
}

void ListScreen::render()
{
  auto _body = Template::createBody();
  visibleCount = _body.height() / (_body.fontHeight() + 2);

  _body.setTextColor(TFT_WHITE);
  const int totalItem = std::min(visibleCount, static_cast<int>(entries.size()) - scrollOffset);
  for (int i = 0; i < totalItem; i++)
  {
    _body.drawString(entries[scrollOffset + i].c_str(), 3, i * _body.fontHeight() + 2 + i * 2);
  }

  selector.createSprite(_body.width(), 12);
  if (previousIndex >= 0 && previousIndex < entries.size())
  {
    const int prevVisible = previousIndex - scrollOffset;
    if (prevVisible >= 0 && prevVisible < visibleCount)
    {
      selector.fillSprite(BLACK);
      selector.setTextColor(TFT_WHITE);
      selector.drawString(entries[previousIndex].c_str(), 3, 2);
      selector.pushSprite(&_body, 0, prevVisible * (_body.fontHeight() + 2));
    }
  }
  const int selVisible = selectedIndex - scrollOffset;
  if (selVisible >= 0 && selVisible < visibleCount)
  {
    selector.fillSprite(BLUE);
    selector.setTextColor(TFT_WHITE);
    selector.drawString(entries[selectedIndex].c_str(), 3, 2);
    selector.pushSprite(&_body, 0, selVisible * (_body.fontHeight() + 2));
  }
  selector.deleteSprite();

  Template::renderBody(&_body);
}

void ListScreen::navigate(const NavAction_t direction, const bool render)
{
  if (direction == NAV_UP && selectedIndex > 0)
  {
    previousIndex = selectedIndex;
    selectedIndex--;
    if (selectedIndex < scrollOffset)
    {
      scrollOffset--;
    }
  }
  if (direction == NAV_DOWN && selectedIndex < entries.size() - 1)
  {
    previousIndex = selectedIndex;
    selectedIndex++;
    if (selectedIndex >= scrollOffset + visibleCount)
    {
      scrollOffset++;
    }
  }
  // Add to ListScreen::navigate
  if (direction == NAV_NEXT && selectedIndex < entries.size() - 1)
  {
    previousIndex = selectedIndex;
    selectedIndex = std::min(selectedIndex + 10, static_cast<int>(entries.size()) - 1);
    if (selectedIndex >= scrollOffset + visibleCount)
    {
      scrollOffset = selectedIndex - visibleCount + 1;
    }
  }
  if (direction == NAV_PREV && selectedIndex > 0)
  {
    previousIndex = selectedIndex;
    selectedIndex = std::max(selectedIndex - 10, 0);
    if (selectedIndex < scrollOffset)
    {
      scrollOffset = selectedIndex;
    }
  }

  if (direction == NAV_ENTER && !entries.empty())
  {
    onEnter(entries[selectedIndex]);
  }
  if (direction == NAV_BACK)
  {
    onBack();
  }

  if (render) this->render();
}

void ListScreen::update()
{
  const auto _keyboard = &M5Cardputer.Keyboard;
  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    if (_keyboard->isKeyPressed(';')) navigate(NAV_UP);
    if (_keyboard->isKeyPressed('.')) navigate(NAV_DOWN);
    if (_keyboard->isKeyPressed(',')) navigate(NAV_PREV);
    if (_keyboard->isKeyPressed('/')) navigate(NAV_NEXT);
    if (_keyboard->isKeyPressed(KEY_ENTER)) navigate(NAV_ENTER, false);
    if (_keyboard->isKeyPressed(KEY_BACKSPACE)) navigate(NAV_BACK, false);
  }
}
