// src/os/screens/FileNavigatorScreen.cpp
#include <SD.h>
#include <utility>

#include "os/screens/FileNavigatorScreen.hpp"

#include "os/component/InputTextScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"

void FileNavigatorScreen::init()
{
  if (_global->getIsSDCardLoaded())
  {
    listDirectory(currentPath);
  }
  else
  {
    Template::renderHead("File Navigator");
    Template::renderStatus("SD Card not loaded", TFT_RED);
    HelperUtility::delayMs(1500);
    _global->setScreen(new MainMenuScreen());
  }
}

void FileNavigatorScreen::preRender(M5Canvas& body)
{
  if (currentState == STATE_MENU)
  {
    if (selectedFile.value == "DIR")
    {
      body.drawString(("Dir: " + selectedFile.label).c_str(), 3, 2);
    }
    else
    {
      body.drawString(("File: " + selectedFile.label).c_str(), 3, 2);
    }
  } else
  {
    if (sourceFullPath != "")
    {
      body.drawString((sourceOperation + ": " + sourceFilename()).c_str(), 3, 2);
    } else
    {
      body.drawString("Press (S) to open menu", 3, 2);
    }
  }
}

void FileNavigatorScreen::listDirectory(const std::string& path)
{
  std::vector<ListEntryItem> files;
  File dir = SD.open(path.c_str());
  if (!dir) return;
  File entry;
  while ((entry = dir.openNextFile()))
  {
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

bool FileNavigatorScreen::copyFile(const std::string& source, const std::string& dest)
{
  File srcFile = SD.open(source.c_str(), FILE_READ);
  if (!srcFile) return false;

  if (SD.exists(dest.c_str()))
  {
    SD.remove(dest.c_str());
  }

  File dstFile = SD.open(dest.c_str(), FILE_WRITE);
  if (!dstFile)
  {
    srcFile.close();
    return false;
  }

  const size_t bufferSize = 512;
  uint8_t buffer[bufferSize];

  while (srcFile.available())
  {
    size_t bytesRead = srcFile.read(buffer, bufferSize);
    dstFile.write(buffer, bytesRead);
  }

  srcFile.close();
  dstFile.close();
  return true;
}

bool FileNavigatorScreen::removeDirectory(const std::string& path)
{
  File dir = SD.open(path.c_str());
  if (!dir || !dir.isDirectory()) return false;

  File entry;
  while ((entry = dir.openNextFile()))
  {
    const std::string entryPath = path + "/" + entry.name();
    if (entry.isDirectory())
    {
      removeDirectory(entryPath);
    } else
    {
      SD.remove(entryPath.c_str());
    }
    entry.close();
  }
  dir.close();
  return SD.rmdir(path.c_str());
}

void FileNavigatorScreen::onEnter(const ListEntryItem& entry)
{
  if (currentState == STATE_FILE)
  {
    const std::string& name = entry.label;
    const std::string newPath = (currentPath == "/" ? "" : currentPath) + "/" + name;
    File f = SD.open(newPath.c_str());
    if (f && f.isDirectory())
    {
      currentPath = newPath;
      listDirectory(currentPath);
    }
    f.close();
  } else if (currentState == STATE_MENU)
  {
    if (entry.label == "Back")
    {
      currentState = STATE_FILE;
      selectedFile = {};
      listDirectory(currentPath);
    } else if (entry.label == "Cancel")
    {
      sourceFullPath = "";
      sourceOperation = "";
      currentState = STATE_FILE;
      selectedFile = {};
      listDirectory(currentPath);
    } else if (entry.label == "Copy" || entry.label == "Cut")
    {
      sourceFullPath = (currentPath == "/" ? "" : currentPath) + "/" + selectedFile.label;
      sourceOperation = entry.label;
      currentState = STATE_FILE;
      selectedFile = {};
      listDirectory(currentPath);
    } else if ((entry.label == "Paste" || entry.label == "Replace") && sourceFullPath != "")
    {
      Template::renderStatus(sourceOperation + " File...", TFT_YELLOW);
      const std::string destPath = (currentPath == "/" ? "" : currentPath) + "/" + sourceFilename();
      if (sourceOperation == "Copy")
      {
        if (!copyFile(sourceFullPath.c_str(), destPath.c_str()))
        {
          Template::renderStatus("Failed to copy file...", TFT_RED);
          HelperUtility::delayMs(1500);
        }
      } else if (sourceOperation == "Cut")
      {
        if (!SD.rename(sourceFullPath.c_str(), destPath.c_str()))
        {
          Template::renderStatus("Failed to move file...", TFT_RED);
          HelperUtility::delayMs(1500);
        }
      }
      sourceFullPath = "";
      sourceOperation = "";
      currentState = STATE_FILE;
      selectedFile = {};
      listDirectory(currentPath);
    } else if (entry.label == "Delete")
    {
      Template::renderStatus("Deleting...", TFT_YELLOW);
      const std::string targetPath = (currentPath == "/" ? "" : currentPath) + "/" + selectedFile.label;
      if (selectedFile.value == "DIR")
      {
        removeDirectory(targetPath);
      } else
      {
        SD.remove(targetPath.c_str());
      }
      currentState = STATE_FILE;
      selectedFile = {};
      listDirectory(currentPath);
    } else if (entry.label == "Rename")
    {
      const std::string newName = InputTextScreen::popup("Rename " + selectedFile.label, selectedFile.label);
      if (newName != "" && newName == selectedFile.label)
      {
        const std::string targetPath = (currentPath == "/" ? "" : currentPath) + "/" + selectedFile.label;
        const std::string newPath = (currentPath == "/" ? "" : currentPath) + "/" + newName;
        if (SD.exists(newPath.c_str()))
        {
          Template::renderStatus("File or directory is already exist...", TFT_RED);
          HelperUtility::delayMs(1500);
        } else
        {
          SD.rename(targetPath.c_str(), newPath.c_str());
        }
      }

      currentState = STATE_FILE;
      selectedFile = {};
      listDirectory(currentPath);
    } else if (entry.label == "New Folder")
    {
      const std::string folderName = InputTextScreen::popup("New Folder");
      const std::string newFolderPath = (currentPath == "/" ? "" : currentPath) + "/" + folderName;
      if (SD.exists(newFolderPath.c_str()))
      {
        Template::renderStatus("Directory is already exist...", TFT_RED);
        HelperUtility::delayMs(1500);
      } else
      {
        SD.mkdir(newFolderPath.c_str());
      }
      currentState = STATE_FILE;
      selectedFile = {};
      listDirectory(currentPath);
    }
  }
}

void FileNavigatorScreen::onBack()
{
  if (currentState == STATE_MENU)
  {
    currentState = STATE_FILE;
    selectedFile = {};
    listDirectory(currentPath);
    return;
  }

  if (currentPath != "/")
  {
    const size_t pos = currentPath.find_last_of('/');
    if (pos != std::string::npos)
    {
      currentPath = currentPath.substr(0, pos);
      if (currentPath.empty()) currentPath = "/";
      listDirectory(currentPath);
    }
  }
  else
  {
    _global->setScreen(new MainMenuScreen());
  }
}

void FileNavigatorScreen::onEscape()
{
  _global->setScreen(new MainMenuScreen());
}

void FileNavigatorScreen::customKeyHandler(Keyboard_Class& keyboard)
{
  if (keyboard.isKeyPressed('s') && currentState == STATE_FILE)
  {
    this->playSound();
    const auto entry = entries[selectedIndex];
    currentState = STATE_MENU;
    selectedFile = entry;
    std::vector<ListEntryItem> baseCommand = {
      {"New Folder"},
      {"Rename"},
      {"Delete"},
    };

    if (entry.value == "FIL")
    {
      baseCommand.push_back({"Copy"});
      baseCommand.push_back({"Cut"});
    }

    if (sourceFullPath != "")
    {
      const std::string destFile = (currentPath == "/" ? "" : currentPath) + "/" + sourceFilename().c_str();
      baseCommand.push_back({SD.exists(destFile.c_str()) ? "Replace" : "Paste"});
      baseCommand.push_back({"Cancel",  sourceOperation});
    }

    baseCommand.push_back({"Back"});
    setEntries(baseCommand);
  }
}
