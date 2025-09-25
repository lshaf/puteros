//
// Created by l-sha on 9/19/2025.
//
#include "os/core/ListScreen.hpp"

void ListScreen::setEntries(const std::vector<ListEntryItem>& newEntries)
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

  if (entries.empty())
  {
    Template::clearBody();
    return;
  }

  _body.setTextColor(TFT_WHITE);
  const int totalItem = std::min(visibleCount, static_cast<int>(entries.size()) - scrollOffset);
  for (int i = 0; i < totalItem; i++)
  {
    const auto selectedIndexVisible = selectedIndex - scrollOffset;
    if (i == selectedIndexVisible)
    {
      _body.fillRect(0, selectedIndexVisible  * _body.fontHeight() + selectedIndexVisible * 2, _body.width(), _body.fontHeight() + 3, BLUE);
    }

    auto item = entries[scrollOffset + i];
    _body.drawString(item.label.c_str(), 3, i * _body.fontHeight() + 2 + i * 2);
    if (!item.value.empty())
    {
      _body.drawRightString(item.value.c_str(), _body.width() - 3, i * _body.fontHeight() + 2 + i * 2);
    }
  }

  Template::renderBody(&_body);
}

void ListScreen::navigate(const NavAction_t direction)
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

  this->render();
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
    if (_keyboard->isKeyPressed(KEY_ENTER) && !entries.empty()) onEnter(entries[selectedIndex]);
    if (_keyboard->isKeyPressed(KEY_BACKSPACE)) onBack();
    if (_keyboard->isKeyPressed('`')) onEscape();
  }
}
