// src/os/FileNavigatorScreen.cpp
#include "os/screens/FileNavigatorScreen.hpp"
#include <SD.h>

FileNavigatorScreen::FileNavigatorScreen(std::string  startPath)
    : currentPath(std::move(startPath)), selector(&M5Cardputer.Lcd) {
    Serial.println("FileNavigatorScreen::FileNavigatorScreen()");
    selector.createSprite(_body.width(), 12);
    listDirectory(currentPath);
}

void FileNavigatorScreen::listDirectory(const std::string& path) {
    entries.clear();
    File dir = SD.open(path.c_str());
    if (!dir) return;
    File entry;
    while ((entry = dir.openNextFile())) {
        entries.emplace_back(entry.name());
        entry.close();
    }
    dir.close();

    _header.fillSprite(TFT_BLACK);
    _header.setTextColor(TFT_WHITE);
    _header.setTextSize(2);
    _header.setCursor(0, 0);
    _header.printf("SDCard: %s", path.c_str());

    _body.fillSprite(BLACK);
    _body.setTextColor(TFT_WHITE);
    for (int i = 0; i < entries.size(); i++) {
        _body.drawString(entries[i].c_str(), 3, i * _body.fontHeight() + 2 + i * 2);
    }

    previousIndex = -1;
    selectedIndex = 0;
    updateSelection();
}

void FileNavigatorScreen::updateSelection()
{
    if (previousIndex >= 0 && previousIndex < entries.size())
    {
        selector.fillSprite(BLACK);
        selector.setTextColor(TFT_WHITE);
        selector.drawString(entries[previousIndex].c_str(), 3, 2);
        selector.pushSprite(&_body, 0, previousIndex * (_body.fontHeight() + 2));
    }

    selector.fillSprite(BLUE);
    selector.setTextColor(TFT_WHITE);
    selector.drawString(entries[selectedIndex].c_str(), 3, 2);
    selector.pushSprite(&_body, 0, selectedIndex * (_body.fontHeight() + 2));
    render();
}

void FileNavigatorScreen::navigate(NavAction_t direction)
{
    if (direction == NAV_UP && selectedIndex > 0)
    {
        previousIndex = selectedIndex;
        selectedIndex--;
    }
    if (direction == NAV_DOWN && selectedIndex < entries.size() - 1)
    {
        previousIndex = selectedIndex;
        selectedIndex++;
    }
    if (direction == NAV_ENTER && !entries.empty())
    {
        const std::string selected = entries[selectedIndex];
        const std::string newPath = (currentPath == "/" ? "" : currentPath) + "/" + selected;
        File f = SD.open(newPath.c_str());
        if (f && f.isDirectory())
        {
            currentPath = newPath;
            listDirectory(currentPath);
        } else
        {
            Serial.println(newPath.c_str());
        }

        f.close();
    }
    if (direction == NAV_BACK && currentPath != "/")
    {
        size_t pos = currentPath.find_last_of('/');
        if (pos != std::string::npos)
        {
            currentPath = currentPath.substr(0, pos);
            if (currentPath.empty()) currentPath = "/";
            listDirectory(currentPath);
        }
    }

    updateSelection();
}

void FileNavigatorScreen::update() {
    auto key = &M5Cardputer.Keyboard;
    if (key->isChange() && key->isPressed())
    {
        if (key->isKeyPressed(';')) navigate(NAV_UP);
        if (key->isKeyPressed('.') && selectedIndex < entries.size() - 1) navigate(NAV_DOWN);
        if (key->isKeyPressed(KEY_ENTER)) navigate(NAV_ENTER);
        if (key->isKeyPressed(KEY_BACKSPACE)) navigate(NAV_BACK);
    }
}
