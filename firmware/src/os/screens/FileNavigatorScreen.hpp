//
// Created by L Shaf on 18/09/25.
//
#pragma once

#pragma once

#include "../core/ListScreen.hpp"
#include <string>

class FileNavigatorScreen final : public ListScreen
{
protected:
  void onEnter(const ListEntryItem& entry) override;
  void onBack() override;
  void onEscape() override;
  int getYOffset() const override { return 12; }
  void preRender(M5Canvas& body) override;
  void customKeyHandler(Keyboard_Class& keyboard) override;

public:
  void init() override;

private:
  std::string currentPath = "/";
  ListEntryItem selectedFile;
  std::string sourceFullPath;
  std::string sourceOperation;
  enum
  {
    STATE_FILE,
    STATE_MENU,
    STATE_LOADING,
  } currentState = STATE_FILE;

  void listDirectory(const std::string& path);
  bool copyFile(const std::string& source, const std::string& dest);
  bool removeDirectory(const std::string& path);
  std::string sourceFilename()
  {
    const size_t pos = sourceFullPath.find_last_of('/');
    if (pos != std::string::npos)
    {
      return sourceFullPath.substr(pos + 1);
    }
    return sourceFullPath;
  }
};
