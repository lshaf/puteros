//
// Created by l-sha on 31/10/2025.
//

#include "os/component/InputFileScreen.h"

void InputFileScreen::init()
{
  Template::renderHead(title);
  auto dir = SD.open(currentPath.c_str());
  if (!dir)
  {
    Template::renderStatus("Failed to open directory", TFT_RED);
    HelperUtility::delayMs(1500);
    done = true;
    return;
  }

  Template::renderStatus("Opening " + currentPath);

  File file;
  std::vector<ListEntryItem> fileEntries;
  while ((file = dir.openNextFile()))
  {
    const std::string name = file.name();
    fileEntries.push_back({
      name,
      file.isDirectory() ? "DIR" : "FIL"
    });
    file.close();
  }
  dir.close();

  setEntries(fileEntries);
}

void InputFileScreen::onEnter(const ListEntryItem& entry)
{
  if (entry.value == "DIR")
  {
    if (currentPath == "/")
      currentPath += entry.label;
    else
      currentPath += "/" + entry.label;

    init();
  } else
  {
    if (currentPath == "/")
      selectedFile = "/" + entry.label;
    else
      selectedFile = currentPath + "/" + entry.label;

    done = true;
  }
}

void InputFileScreen::onBack()
{
  if (currentPath == initPath || currentPath == "/")
  {
    done = true;
    selectedFile = "";
    return;
  }

  const auto lastSlash = currentPath.find_last_of('/');
  if (lastSlash == 0)
    currentPath = "/";
  else
    currentPath = currentPath.substr(0, lastSlash);

  init();
}

void InputFileScreen::onEscape()
{
  done = true;
  selectedFile = "";
}

std::string InputFileScreen::popup(const std::string& title, const std::string& initial)
{
  const auto screen = new InputFileScreen(title, initial);
  while (!screen->isDone())
  {
    M5Cardputer.update();
    screen->update();
    delay(10);
  }

  return screen->getResult();
}
