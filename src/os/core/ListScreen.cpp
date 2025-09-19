//
// Created by l-sha on 9/19/2025.
//
#include "os/core//ListScreen.hpp"

ListScreen::ListScreen() : selector(&M5Cardputer.Lcd) {
    selector.createSprite(_body.width(), 12);
    _body.setTextScroll(true);
}

void ListScreen::setEntries(const std::vector<std::string>& newEntries) {
    entries = newEntries;
    currentPage = 0;
    selectedIndex = 0;
    previousIndex = -1;
    showList();
    updateSelection();
}

void ListScreen::showList() {
    _body.fillSprite(BLACK);
    _body.setTextColor(TFT_WHITE);
    const int totalItem = std::min(perPage, static_cast<int>(entries.size()) - currentPage * perPage);
    for (int i = 0; i < totalItem; i++) {
        _body.drawString(entries[currentPage * perPage + i].c_str(), 3, i * _body.fontHeight() + 2 + i * 2);
    }
}

void ListScreen::updateSelection() {
    if (previousIndex >= 0 && previousIndex < entries.size()) {
        selector.fillSprite(BLACK);
        selector.setTextColor(TFT_WHITE);
        selector.drawString(entries[currentPage * perPage + previousIndex].c_str(), 3, 2);
        selector.pushSprite(&_body, 0, previousIndex * (_body.fontHeight() + 2));
    }
    selector.fillSprite(BLUE);
    selector.setTextColor(TFT_WHITE);
    selector.drawString(entries[currentPage * perPage + selectedIndex].c_str(), 3, 2);
    selector.pushSprite(&_body, 0, selectedIndex * (_body.fontHeight() + 2));
    render();
}

void ListScreen::navigate(NavAction_t direction) {
    if (direction == NAV_UP && currentPage * perPage + selectedIndex > 0) {
        previousIndex = selectedIndex;
        selectedIndex--;
        if (selectedIndex < 0) {
            currentPage--;
            selectedIndex = perPage - 1;
            previousIndex = -1;
            showList();
        }
    }
    if (direction == NAV_DOWN && currentPage * perPage + selectedIndex < entries.size() - 1) {
        previousIndex = selectedIndex;
        selectedIndex++;
        if (selectedIndex >= perPage) {
            currentPage++;
            selectedIndex = 0;
            previousIndex = -1;
            showList();
        }
    }
    if (direction == NAV_PREV && currentPage > 0) {
        currentPage--;
        previousIndex = -1;
        selectedIndex = 0;
        showList();
    }
    if (direction == NAV_NEXT && (currentPage + 1) * perPage < entries.size()) {
        currentPage++;
        previousIndex = -1;
        selectedIndex = 0;
        showList();
    }
    if (direction == NAV_ENTER && !entries.empty()) {
        onEnter(entries[currentPage * perPage + selectedIndex]);
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
