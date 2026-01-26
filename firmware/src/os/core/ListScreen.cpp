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
  if (entries.empty())
  {
    Template::clearBody();
    return;
  }

  auto _body = Template::createBody();
  preRender(_body);

  _body.setTextSize(1);
  _body.setTextColor(TFT_WHITE);

  // Calculate basic metrics
  const int itemHeight = _body.fontHeight() + 4; // fontHeight + padding
  const int availableHeight = _body.height() - getYOffset();

  // Calculate maximum items that can fit (no partial items)
  visibleCount = availableHeight / itemHeight;

  // If there are fewer entries than can fit, just show them all
  if (entries.size() <= visibleCount) {
    scrollOffset = 0;
    visibleCount = entries.size();
  } else {
    // Adjust scroll to ensure selected item is fully visible
    if (selectedIndex < scrollOffset) {
      // Selected item is above current view
      scrollOffset = selectedIndex;
    } else if (selectedIndex >= scrollOffset + visibleCount) {
      // Selected item is below current view
      scrollOffset = selectedIndex - visibleCount + 1;
    }

    // Ensure scrollOffset stays within bounds
    if (scrollOffset + visibleCount > entries.size()) {
      scrollOffset = entries.size() - visibleCount;
    }
    if (scrollOffset < 0) {
      scrollOffset = 0;
    }
  }

  // Draw selection highlight
  const int selectedIndexVisible = selectedIndex - scrollOffset;
  if (selectedIndexVisible >= 0 && selectedIndexVisible < visibleCount) {
    _body.fillRect(
      0,
      getYOffset() + selectedIndexVisible * itemHeight,
      _body.width(),
      itemHeight,
      _global->getMainColor()
    );
  }

  // Draw all visible items
  int actualShowItem = visibleCount;
  if (_body.height() - visibleCount * itemHeight > 3) actualShowItem++;
  actualShowItem = std::min(actualShowItem, static_cast<int>(entries.size()) - scrollOffset);
  for (int i = 0; i < actualShowItem; ++i) {
    const auto &item = entries[scrollOffset + i];
    const int y = getYOffset() + i * itemHeight;

    _body.drawString(item.label.c_str(), 3, y + 2);
    if (!item.value.empty()) {
      _body.drawRightString(item.value.c_str(), _body.width() - 3, y + 2);
    }
  }

  Template::renderBody(&_body);
}

void ListScreen::navigate(const NavAction_t direction)
{
  const int totalEntries = static_cast<int>(entries.size());
  if (totalEntries == 0) return;
  if (direction == NAV_UP && selectedIndex > 0) {
    selectedIndex--;
  } else if (direction == NAV_DOWN && selectedIndex < totalEntries - 1) {
    selectedIndex++;
  } else if (direction == NAV_NEXT && selectedIndex < totalEntries - 1) {
    // Jump by visibleCount items or to last item
    selectedIndex = std::min(selectedIndex + visibleCount, totalEntries - 1);
  } else if (direction == NAV_PREV && selectedIndex > 0) {
    // Jump by visibleCount items or to first item
    selectedIndex = std::max(selectedIndex - visibleCount, 0);
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
    else if (_keyboard->isKeyPressed('.')) navigate(NAV_DOWN);
    else if (_keyboard->isKeyPressed(',')) navigate(NAV_PREV);
    else if (_keyboard->isKeyPressed('/')) navigate(NAV_NEXT);
    else if (_keyboard->isKeyPressed(KEY_ENTER) && !entries.empty())
    {
      this->playSound();
      onEnter(entries[selectedIndex]);
    } else if (_keyboard->isKeyPressed(KEY_BACKSPACE))
    {
      this->playSound();
      onBack();
    } else if (_keyboard->isKeyPressed('`'))
    {
      this->playSound();
      onEscape();
    } else
    {
      customKeyHandler(*_keyboard);
    }
  }
}
