// src/os/screens/FileNavigatorScreen.cpp
#include <SD.h>
#include <utility>

#include "os/screens/FileNavigatorScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"

FileNavigatorScreen::FileNavigatorScreen(std::string  path)
    : currentPath(std::move(path)) {}

void FileNavigatorScreen::init()
{
    ListScreen::init();
    listDirectory(currentPath);
}


void FileNavigatorScreen::listDirectory(const std::string& path) {
    std::vector<std::string> files;
    File dir = SD.open(path.c_str());
    if (!dir) return;
    File entry;
    while ((entry = dir.openNextFile())) {
        files.emplace_back(entry.name());
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

void FileNavigatorScreen::onEnter(const std::string& entry) {
    const std::string newPath = (currentPath == "/" ? "" : currentPath) + "/" + entry;
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

void FileNavigatorScreen::update() {
    ListScreen::update();
}
