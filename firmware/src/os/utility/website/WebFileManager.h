//
// Created by L Shaf on 17/12/25.
//

#pragma once

#include <ESPAsyncWebServer.h>
#include "os/GlobalState.hpp"

class WebFileManager
{
public:
  WebFileManager() : _global(&GlobalState::getInstance()) {}
  bool begin();
  bool end();

  String getError()
  {
    return lastError;
  }
private:
  int SESSION_COUNTER = 0;
  static constexpr int MAX_SESSIONS = 10;
  String activeSessions[MAX_SESSIONS];
  AsyncWebServer server{80};
  File fsUpload;

  GlobalState* _global;

  String lastError;
  void prepareServer();
  bool isAuthenticated(const AsyncWebServerRequest* request, bool logout = false);
  bool removeDirectory(const String& path);

  String getContentType(const String& filename)
  {
    if(filename.endsWith(".htm")) return "text/html";
    if(filename.endsWith(".html")) return "text/html";
    if(filename.endsWith(".css")) return "text/css";
    if(filename.endsWith(".js")) return "application/javascript";
    if(filename.endsWith(".png")) return "image/png";
    if(filename.endsWith(".gif")) return "image/gif";
    if(filename.endsWith(".jpg")) return "image/jpeg";
    if(filename.endsWith(".ico")) return "image/x-icon";
    if(filename.endsWith(".xml")) return "text/xml";
    if(filename.endsWith(".pdf")) return "application/x-pdf";
    if(filename.endsWith(".zip")) return "application/x-zip";
    if(filename.endsWith(".gz")) return "application/x-gzip";
    return "text/plain";
  }
};
