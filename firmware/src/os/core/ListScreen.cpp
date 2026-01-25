//
// Created by l-sha on 9/19/2025.
//
#include "os/core/ListScreen.hpp"
#include "os/utility/AudioUtility.h"

void ListScreen::setEntries(const std::vector<ListEntryItem>& newEntries)
{
  entries = std::move(newEntries);
  selectedIndex = 0;
  scrollOffset = 0;
  render();
}

void ListScreen::render()
{
  auto _body = Template::createBody();
  preRender(_body);

  if (entries.empty())
  {
    Template::clearBody();
    return;
  }

  _body.setTextSize(1);
  _body.setTextColor(TFT_WHITE);
  visibleCount = (_body.height() - getYOffset()) / (_body.fontHeight() + 4);
  const int totalItem = std::min(visibleCount, static_cast<int>(entries.size()) - scrollOffset);
  const int lineHeight = _body.fontHeight() + 2;
  const int selectedIndexVisible = selectedIndex - scrollOffset;
  if (selectedIndexVisible >= 0 && selectedIndexVisible < totalItem)
  {
    _body.fillRect(
      0,
      getYOffset() + selectedIndexVisible * lineHeight + selectedIndexVisible * 2,
      _body.width(),
      _body.fontHeight() + 3,
      _global->getMainColor()
    );
  }

  for (int i = 0; i < totalItem; ++i)
  {
    const auto &item = entries[scrollOffset + i];
    const int y = getYOffset() + i * lineHeight + i * 2;

    _body.drawString(item.label.c_str(), 3, y + 2);
    if (!item.value.empty())
    {
      _body.drawRightString(item.value.c_str(), _body.width() - 3, y + 2);
    }
  }

  Template::renderBody(&_body);
}

void ListScreen::navigate(const NavAction_t direction)
{
  if (direction == NAV_UP && selectedIndex > 0)
  {
    selectedIndex--;
    if (selectedIndex < scrollOffset)
    {
      scrollOffset--;
    }
  }
  if (direction == NAV_DOWN && selectedIndex < entries.size() - 1)
  {
    selectedIndex++;
    if (selectedIndex >= scrollOffset + visibleCount)
    {
      scrollOffset++;
    }
  }
  // Add to ListScreen::navigate
  if (direction == NAV_NEXT && selectedIndex < entries.size() - 1)
  {
    selectedIndex = std::min(selectedIndex + 10, static_cast<int>(entries.size()) - 1);
    if (selectedIndex >= scrollOffset + visibleCount)
    {
      scrollOffset = selectedIndex - visibleCount + 1;
    }
  }
  if (direction == NAV_PREV && selectedIndex > 0)
  {
    selectedIndex = std::max(selectedIndex - 10, 0);
    if (selectedIndex < scrollOffset)
    {
      scrollOffset = selectedIndex;
    }
  }

  this->playSound();
  this->render();
}

void ListScreen::playSound()
{
  const bool navSound = _config->get(APP_CONFIG_NAV_SOUND, APP_CONFIG_NAV_SOUND_DEFAULT).toInt();
  if (navSound) AudioUtility::playRandomTone();
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
    if (_keyboard->isKeyPressed(KEY_ENTER) && !entries.empty())
    {
      this->playSound();
      onEnter(entries[selectedIndex]);
    }
    if (_keyboard->isKeyPressed(KEY_BACKSPACE))
    {
      this->playSound();
      onBack();
    }
    if (_keyboard->isKeyPressed('`'))
    {
      this->playSound();
      onEscape();
    }
  }
}
