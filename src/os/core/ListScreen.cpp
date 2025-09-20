//
// Created by l-sha on 9/19/2025.
//
#include "os/core/ListScreen.hpp"

ListScreen::ListScreen() : selector(&M5Cardputer.Lcd) {
    selector.createSprite(_body.width(), 12);
    visibleCount = _body.height() / (_body.fontHeight() + 2);
}

void ListScreen::setEntries(const std::vector<std::string>& newEntries) {
    entries = newEntries;
    currentPage = 0;
    selectedIndex = 0;
    previousIndex = -1;
    scrollOffset = 0;
    showList();
    updateSelection();
}

void ListScreen::showList() {
    _body.fillSprite(BLACK);
    _body.setTextColor(TFT_WHITE);
    const int totalItem = std::min(visibleCount, static_cast<int>(entries.size()) - scrollOffset);
    for (int i = 0; i < totalItem; i++) {
        _body.drawString(entries[scrollOffset + i].c_str(), 3, i * _body.fontHeight() + 2 + i * 2);
    }
}

void ListScreen::updateSelection() {
    if (previousIndex >= 0 && previousIndex < entries.size()) {
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
    if (selVisible >= 0 && selVisible < visibleCount) {
        selector.fillSprite(BLUE);
        selector.setTextColor(TFT_WHITE);
        selector.drawString(entries[selectedIndex].c_str(), 3, 2);
        selector.pushSprite(&_body, 0, selVisible * (_body.fontHeight() + 2));
    }
    render();
}

void ListScreen::navigate(const NavAction_t direction) {
    if (direction == NAV_UP && selectedIndex > 0) {
        previousIndex = selectedIndex;
        selectedIndex--;
        if (selectedIndex < scrollOffset) {
            scrollOffset--;
            showList();
        }
    }
    if (direction == NAV_DOWN && selectedIndex < entries.size() - 1) {
        previousIndex = selectedIndex;
        selectedIndex++;
        if (selectedIndex >= scrollOffset + visibleCount) {
            scrollOffset++;
            showList();
        }
    }
    // Add to ListScreen::navigate
    if (direction == NAV_NEXT && selectedIndex < entries.size() - 1) {
        previousIndex = selectedIndex;
        selectedIndex = std::min(selectedIndex + 10, static_cast<int>(entries.size()) - 1);
        if (selectedIndex >= scrollOffset + visibleCount) {
            scrollOffset = selectedIndex - visibleCount + 1;
            showList();
        }
    }
    if (direction == NAV_PREV && selectedIndex > 0) {
        previousIndex = selectedIndex;
        selectedIndex = std::max(selectedIndex - 10, 0);
        if (selectedIndex < scrollOffset) {
            scrollOffset = selectedIndex;
            showList();
        }
    }
    if (direction == NAV_ENTER && !entries.empty()) {
        onEnter(entries[selectedIndex]);
    }
    if (direction == NAV_BACK) {
        onBack();
    }
    updateSelection();
}

void ListScreen::update() {
    if (_keyboard->isChange() && _keyboard->isPressed()) {
        if (_keyboard->isKeyPressed(';')) navigate(NAV_UP);
        if (_keyboard->isKeyPressed('.')) navigate(NAV_DOWN);
        if (_keyboard->isKeyPressed(',')) navigate(NAV_PREV);
        if (_keyboard->isKeyPressed('/')) navigate(NAV_NEXT);
        if (_keyboard->isKeyPressed(KEY_ENTER)) navigate(NAV_ENTER);
        if (_keyboard->isKeyPressed(KEY_BACKSPACE)) navigate(NAV_BACK);
    }
}
