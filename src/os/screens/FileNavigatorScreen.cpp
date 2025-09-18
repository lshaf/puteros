// src/os/FileNavigatorScreen.cpp
#include "os/screens/FileNavigatorScreen.hpp"
#include <SD.h>

#include <utility>


FileNavigatorScreen::FileNavigatorScreen(std::string  startPath)
    : currentPath(std::move(startPath)), canvas(&M5Cardputer.Lcd) {
    listDirectory(currentPath);
    canvas.createSprite(M5.Lcd.width(), 10);
}

void FileNavigatorScreen::listDirectory(const std::string& path) {
    entries.clear();
    File dir = SD.open(path.c_str());
    if (!dir) return;
    File entry;
    while ((entry = dir.openNextFile())) {
        entries.push_back(entry.name());
        entry.close();
    }
    dir.close();
    previousIndex = -1;
    selectedIndex = 0;
    updateSelection();
}

void FileNavigatorScreen::updateSelection()
{
    if (previousIndex >= 0 && previousIndex < entries.size())
    {
        canvas.fillSprite(BLACK);
        canvas.setTextColor(TFT_WHITE);
        canvas.drawString(entries[previousIndex].c_str(), 1, 1);
        canvas.pushSprite(0, 10 + (previousIndex * 10));
    }

    canvas.fillSprite(BLUE);
    canvas.setTextColor(TFT_WHITE);
    canvas.drawString(entries[selectedIndex].c_str(), 1, 1);
    canvas.pushSprite(0, 10 + (selectedIndex * 10));
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
        const std::string newPath = currentPath + "/" + selected;
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

    Serial.print("CurrentIndex: ");
    Serial.println(selectedIndex);
    updateSelection();
}



void FileNavigatorScreen::update() {
    auto key = &M5Cardputer.Keyboard;
    if (key->isChange() && key->isPressed())
    {
        if (key->isKeyPressed(';')) navigate(NAV_UP);
        if (key->isKeyPressed('.') && selectedIndex < entries.size() - 1) navigate(NAV_DOWN);
        if (key->isKeyPressed(KEY_ENTER)) navigate(NAV_ENTER);
    }
}

void FileNavigatorScreen::render() {
    const auto display = &M5Cardputer.Lcd;
    display->fillScreen(TFT_BLACK);
    display->setCursor(1, 1);
    display->println(("SDCard: " + currentPath).c_str());
    for (size_t i = 0; i < entries.size(); ++i) {
        display->setCursor(1, 10 + ((i * 10) + 1));
        display->println(entries[i].c_str());
    }
}
