// src/os/screens/FileNavigatorScreen.cpp
#include <SD.h>
#include <utility>

#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"

FileNavigatorScreen::FileNavigatorScreen(std::string  path)
    : currentPath(std::move(path)) {}

void FileNavigatorScreen::init()
{
    if (_global->getIsSDCardLoaded())
    {
        listDirectory(currentPath);
    } else
    {
        Template::renderHead("File Navigator");
        Template::renderStatus("SD Card not loaded", TFT_RED);
        HelperUtility::delayMs(1500);
        _global->setScreen(new MainMenuScreen());
    }
}


void FileNavigatorScreen::listDirectory(const std::string& path) {
    std::vector<ListEntryItem> files;
    File dir = SD.open(path.c_str());
    if (!dir) return;
    File entry;
    while ((entry = dir.openNextFile())) {
        const std::string type = !entry.isDirectory() ? "FIL" : "DIR";
        files.push_back({entry.name(), type});
        entry.close();
    }
    dir.close();

    currentPath = path;

    std::string title;
    const size_t pos = path.find_last_of('/');
    if (pos == std::string::npos || pos == path.length() - 1) title = "/";
    else title = path.substr(pos + 1);

    Template::renderHead(title);
    setEntries(files);
}

void FileNavigatorScreen::onEnter(ListEntryItem entry) {
    const std::string& name = entry.label;
    const std::string newPath = (currentPath == "/" ? "" : currentPath) + "/" + name;
    File f = SD.open(newPath.c_str());
    if (f && f.isDirectory()) {
        currentPath = newPath;
        listDirectory(currentPath);
    } else {
        Serial.println(newPath.c_str());
    }
    f.close();
}

void FileNavigatorScreen::onBack() {
    if (currentPath != "/") {
        const size_t pos = currentPath.find_last_of('/');
        if (pos != std::string::npos) {
            currentPath = currentPath.substr(0, pos);
            if (currentPath.empty()) currentPath = "/";
            listDirectory(currentPath);
        }
    } else
    {
        _global->setScreen(new MainMenuScreen());
    }
}

void FileNavigatorScreen::onEscape() {
    _global->setScreen(new MainMenuScreen());
}

void FileNavigatorScreen::update() {
    ListScreen::update();
}
