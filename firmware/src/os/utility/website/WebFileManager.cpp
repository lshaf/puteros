//
// Created by L Shaf on 17/12/25.
//

#include "WebFileManager.h"

#include <WiFi.h>

bool WebFileManager::begin()
{
  const wifi_mode_t mode = WiFi.getMode();
  const bool apActive = (mode == WIFI_MODE_AP || mode == WIFI_MODE_APSTA);
  const bool staConnected = (mode == WIFI_MODE_STA || mode == WIFI_MODE_APSTA) &&
                      WiFi.status() == WL_CONNECTED;

  if (!apActive && !staConnected)
  {
    lastError = "WiFi not connected in any network";
    return false;
  }

  if (!_global->getIsSDCardLoaded())
  {
    lastError = "SDCard not loaded";
    return false;
  }

  const String indexPath = "/m5geek/web/file_manager/index.htm";
  if (SD.exists(indexPath) == false)
  {
    lastError = indexPath + " not found";
    return false;
  }

  prepareServer();
  return true;
}

bool WebFileManager::end()
{
  if (fsUpload) fsUpload.close();
  _global->getServer()->reset();
  return true;
}

bool WebFileManager::isAuthenticated(const AsyncWebServerRequest* request, bool logout)
{
  if (!request->hasHeader("Cookie")) {
    return false;
  }

  const String cookies = request->header("cookie");
  int sessionStart = cookies.indexOf("session=");
  if (sessionStart == -1) {
    return false;
  }

  sessionStart += 8;
  const int sessionEnd = cookies.indexOf(';', sessionStart);
  const String sessionToken = (sessionEnd == -1)
    ? cookies.substring(sessionStart)
    : cookies.substring(sessionStart, sessionEnd);

  for (int i = 0; i < MAX_SESSIONS; i++) {
    if (activeSessions[i] == sessionToken) {
      if (logout) activeSessions[i] = "";
      return true;
    }
  }
  return false;
}

bool WebFileManager::removeDirectory(const String& path)
{
  File dir = SD.open(path);
  if (!dir || !dir.isDirectory()) {
    return false;
  }

  File file = dir.openNextFile();
  while (file) {
    String filePath = String(file.path());
    if (file.isDirectory()) {
      removeDirectory(filePath);
    } else {
      SD.remove(filePath);
    }
    file = dir.openNextFile();
  }
  dir.close();

  return SD.rmdir(path);
}

void WebFileManager::prepareServer()
{
  const auto server = _global->getServer();
  server->on("/download", HTTP_GET, [this](AsyncWebServerRequest *request)
  {
    if (!isAuthenticated(request))
    {
      request->send(401, "text/plain", "not authenticated.");
      return;
    }

    if (!request->hasArg("file"))
    {
      request->send(400, "text/plain", "No file specified.");
      return;
    }

    const auto filePath = request->arg("file");
    if (SD.exists(filePath) == false)
    {
      request->send(404, "text/plain", "File not found.");
      return;
    }

    request->send(SD, filePath, getContentType(filePath), true);
  });
  server->on("/upload", HTTP_POST, [this](AsyncWebServerRequest *request)
  {
    if (!isAuthenticated(request))
    {
      request->send(401, "text/plain", "not authenticated.");
      return;
    }

    request->send(200, "text/plain", "ok.");
  }, [this](AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
  {
    if (!isAuthenticated(request))
    {
      request->send(401, "text/plain", "not authenticated.");
      return;
    }

    if (!index) {
      // first pass
      String path = "/";
      if (request->hasArg("folder"))
      {
        path = request->arg("folder");
        if (!path.startsWith("/")) path = "/" + path;
        if (!path.endsWith("/")) path += "/";
      }

      const String fullPath = path + filename;
      const int lastSlash = fullPath.lastIndexOf('/');
      if (lastSlash > 0) {
        const String dirPath = fullPath.substring(0, lastSlash);
        HelperUtility::makeDirectoryRecursive(dirPath.c_str());
      }

      fsUpload = SD.open(fullPath, FILE_WRITE);
    }

    if (len) {
      if (fsUpload) fsUpload.write(data, len);
    }

    if (final)
    {
      if (fsUpload) fsUpload.close();
    }
  });

  const auto currentPassword = _global->getConfig()->get(APP_CONFIG_WIFI_WEB_PASSWORD, APP_CONFIG_WIFI_WEB_PASSWORD_DEFAULT);
  server->on("/", HTTP_POST, [this, currentPassword](AsyncWebServerRequest* request)
  {
    if (!request->hasParam("command", true))
    {
      request->send(404, "text/plain", "404");
      return;
    }

    const auto command = request->getParam("command", true)->value();
    if (command != "sudo" && !isAuthenticated(request))
    {
      request->send(401, "text/plain", "not authenticated.");
      return;
    }

    if (command == "ls")
    {
      String currentPath = request->getParam("path", true)->value();
      if (currentPath == "") currentPath = "/";

      String resp = "";
      File dir = SD.open(currentPath);
      if (!dir || !dir.isDirectory())
      {
        request->send(403, "text/plain", "Not a directory.");
        return;
      }

      File file = dir.openNextFile();
      while (file)
      {
        resp += (file.isDirectory() ? "DIR:" : "FILE:") + String(file.name()) + ":" + String(file.size()) + "\n";
        file = dir.openNextFile();
      }

      request->send(200, "text/plain", resp);
    } else if (command == "sysinfo")
    {
      String resp = "M5Geek File Manager\n";
      resp += "FS:" + String(SD.totalBytes() - SD.usedBytes()) + "\n";
      resp += "US:" + String(SD.usedBytes()) + "\n";
      resp += "TS:" + String(SD.totalBytes()) + "\n";
      resp += "VR:" + String(APP_VERSION) + "\n";
      request->send(200, "text/plain", resp);
    } else if (command == "sudo")
    {
      const auto password = request->getParam("param", true)->value();
      if (password.compareTo(currentPassword) == 0)
      {
        const auto sessionToken = String(random(0x7FFFFFFF), HEX);
        SESSION_COUNTER = (SESSION_COUNTER + 1) % MAX_SESSIONS;
        activeSessions[SESSION_COUNTER] = sessionToken;

        // Set HTTPOnly cookie (Max-Age in seconds)
        AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", "Login successful");
        resp->addHeader(AsyncWebHeader::parse("Set-Cookie: session=" + sessionToken + "; HttpOnly; Max-Age=3600"));
        request->send(resp);
      } else
      {
        request->send(403, "text/plain", "forbidden");
      }
    } else if (command == "exit")
    {
      isAuthenticated(request, true);
      AsyncWebServerResponse *resp = request->beginResponse(200, "text/plain", "Logged out");
      resp->addHeader("Set-Cookie", "session=; HttpOnly; Expires=Thu, 01 Jan 1970 00:00:00 GMT; Path=/");
      request->send(resp);
    } else if (command == "rm")
    {
      String currentPath = request->getParam("path", true)->value();
      if (currentPath == "")
      {
        request->send(400, "text/plain", "No file specified.");
        return;
      }

      if (SD.exists(currentPath) == false)
      {
        request->send(404, "text/plain", "File not found.");
        return;
      }

      File dir = SD.open(currentPath);
      if (!dir.isDirectory()) {
        dir.close();
        if (SD.remove(currentPath))
        {
          request->send(200, "text/plain", "File deleted.");
        } else
        {
          request->send(500, "text/plain", "Failed to delete file.");
        }
      } else
      {
        dir.close();
        if (removeDirectory(currentPath))
        {
          request->send(200, "text/plain", "Directory deleted.");
        } else
        {
          request->send(500, "text/plain", "Failed to delete directory.");
        }
      }
    } else if (command == "mv")
    {
      const auto sourcePath = request->getParam("src", true)->value();
      const auto destPath = request->getParam("dst", true)->value();
      if (sourcePath == "" || destPath == "")
      {
        request->send(400, "text/plain", "Source or destination path not specified.");
        return;
      }

      if (SD.exists(sourcePath) == false)
      {
        request->send(404, "text/plain", "Source file not found.");
        return;
      }

      if (SD.rename(sourcePath, destPath))
      {
        request->send(200, "text/plain", "File moved/renamed.");
      } else
      {
        request->send(500, "text/plain", "Failed to move/rename file.");
      }
    } else if (command == "mkdir")
    {
      const auto dirPath = request->getParam("path", true)->value();
      if (dirPath == "")
      {
        request->send(400, "text/plain", "No directory name specified.");
        return;
      }

      if (SD.mkdir(dirPath))
      {
        request->send(200, "text/plain", "Directory created.");
      } else
      {
        request->send(500, "text/plain", "Failed to create directory.");
      }
    } else if (command == "touch")
    {
      const auto filePath = request->getParam("path", true)->value();
      if (filePath == "")
      {
        request->send(400, "text/plain", "No file name specified.");
        return;
      }

      File file = SD.open(filePath, FILE_WRITE);
      if (file)
      {
        file.close();
        request->send(200, "text/plain", "File created.");
      } else
      {
        request->send(500, "text/plain", "Failed to create file.");
      }
    } else if (command == "cat")
    {
      const auto filePath = request->getParam("path", true)->value();
      if (filePath == "")
      {
        request->send(400, "text/plain", "No file name specified.");
        return;
      }
      if (SD.exists(filePath) == false)
      {
        request->send(404, "text/plain", "File not found.");
        return;
      }

      File file = SD.open(filePath);
      String resp = "";
      while (file.available())
      {
        resp += char(file.read());
      }
      file.close();
      request->send(200, "text/plain", resp);
    } else if (command == "echo")
    {
      const auto filePath = request->getParam("path", true)->value();
      const auto content = request->getParam("content", true)->value();
      if (filePath == "" || content == "")
      {
        request->send(400, "text/plain", "File name or content not specified.");
        return;
      }
      File file = SD.open(filePath, FILE_WRITE);
      if (!file)
      {
        request->send(500, "text/plain", "Failed to open file.");
        return;
      }

      file.print(content);
      file.close();
      request->send(200, "text/plain", "Content written to file.");
    }
    else
    {
      request->send(404, "text/plain", "command not found");
    }
  });

  server->onNotFound([](AsyncWebServerRequest* request)
  {
    request->send(404, "text/plain", "404");
  });

  server->on("/theme.css", HTTP_GET, [this](AsyncWebServerRequest* request)
  {
    const auto priColor = _global->getMainColor();
    const String css = ":root{--color:" + color565ToWebHex(priColor) + ";}";
    AsyncWebServerResponse *themeResponse = request->beginResponse(200, "text/css", css);
    request->send(themeResponse);
  });

  server->serveStatic("/", SD, "/m5geek/web/file_manager/");
}