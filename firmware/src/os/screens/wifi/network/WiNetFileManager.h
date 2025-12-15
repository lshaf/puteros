//
// Created by l-sha on 08/12/2025.
//
#pragma once

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "os/core/ListScreen.hpp"

class WiNetFileManager final : public ListScreen
{
public:
  void init() override;

protected:
  int SESSION_COUNTER = 0;
  static constexpr int MAX_SESSIONS = 10;
  String activeSessions[MAX_SESSIONS];
  AsyncWebServer server{80};
  String currentPassword;
  File fsUpload;

  enum State_e
  {
    STATE_MENU,
    STATE_WEB_MANAGER
  } currentState = STATE_MENU;

  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void update() override;

  void prepareServer();
  void renderMainMenu();

  String getContentType(const String& filename);
  std::vector<String> getBodyCommands(const String& body);
  bool isAuthenticated(const AsyncWebServerRequest* request, bool logout = false);
  bool removeDirectory(const String& path);
};
