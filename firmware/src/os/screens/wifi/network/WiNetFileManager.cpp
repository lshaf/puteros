//
// Created by l-sha on 08/12/2025.
//

#include "WiNetFileManager.h"

#include "os/component/InputTextScreen.hpp"
#include "os/screens/wifi/WifiNetworkScreen.h"
#include <WiFi.h>

void WiNetFileManager::init()
{
  Template::renderHead("Web File Manager");
  if (!_global->getIsSDCardLoaded())
  {
    Template::renderStatus("SD Card is not loaded", TFT_RED);
    HelperUtility::delayMs(2000);
    return;
  }

  currentPassword = _global->getConfig()
    ->get(APP_CONFIG_WIFI_WEB_PASSWORD, APP_CONFIG_WIFI_WEB_PASSWORD_DEFAULT);
  renderMainMenu();
}

void WiNetFileManager::onBack()
{
  _global->setScreen(new WifiNetworkScreen());
}

void WiNetFileManager::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MENU)
  {
    if (entry.label == "Start")
    {
      prepareServer();
    } else if (entry.label == "Password")
    {
      const auto newPassword = InputTextScreen::popup("Web Password", currentPassword.c_str());
      if (newPassword.empty())
      {
        Template::renderStatus("Password is required");
        HelperUtility::delayMs(2000);
      } else
      {
        auto cleanPassword = String(newPassword.c_str());
        cleanPassword.trim();
        _global->getConfig()->set(APP_CONFIG_WIFI_WEB_PASSWORD, cleanPassword);
        _global->getConfig()->save();
        currentPassword = newPassword.c_str();
      }

      renderMainMenu();
    }
  }
}

String WiNetFileManager::getContentType(const String& filename) {
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

std::vector<String> WiNetFileManager::getBodyCommands(const String& body){
  std::vector<String> lines;
  lines.reserve(8);
  String cur;
  cur.reserve(64);

  for (size_t i = 0; i < (size_t) body.length(); ++i) {
    char c = body.charAt(i);
    if (c == '\r') continue;
    if (c == '\n') {
      lines.push_back(cur);
      cur = "";
    } else {
      cur += c;
    }
  }
  if (cur.length() > 0) lines.push_back(cur);
  return lines;
}

bool WiNetFileManager::isAuthenticated(const AsyncWebServerRequest* request, bool logout)
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

bool WiNetFileManager::removeDirectory(const String& path)
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

void WiNetFileManager::prepareServer()
{
  currentState = STATE_WEB_MANAGER;
  if (WiFi.status() != WL_CONNECTED)
  {
    Template::renderStatus("WiFi not connected", TFT_RED);
    HelperUtility::delayMs(2000);
    renderMainMenu();
    return;
  }

  if (!_global->getIsSDCardLoaded())
  {
    Template::renderStatus("SDCard is not mounted", TFT_RED);
    HelperUtility::delayMs(2000);
    renderMainMenu();
    return;
  }

  const String indexPath = "/puteros/web/file_manager/index.htm";
  if (SD.exists(indexPath) == false)
  {
    Template::renderStatus((indexPath + " not found").c_str(), TFT_RED);
    HelperUtility::delayMs(2000);
    renderMainMenu();
    return;
  }

  Template::renderStatus("Starting Server...");
  server.on("/download", HTTP_GET, [this](AsyncWebServerRequest *request)
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
  server.on("/upload", HTTP_POST, [this](AsyncWebServerRequest *request)
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

  server.on("/", HTTP_POST, [this](AsyncWebServerRequest* request)
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
      String resp = "PuterOS File Manager\n";
      resp += "FS:" + String(SD.totalBytes() - SD.usedBytes()) + "\n";
      resp += "US:" + String(SD.usedBytes()) + "\n";
      resp += "TS:" + String(SD.totalBytes()) + "\n";
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

  server.onNotFound([](AsyncWebServerRequest* request)
  {
    request->send(404, "text/plain", "404");
  });

  server.serveStatic("/", SD, "/puteros/web/file_manager/");

  server.begin();
  Template::renderStatus(("http://" + WiFi.localIP().toString() + "/").c_str(), TFT_GREEN);
}


void WiNetFileManager::renderMainMenu()
{
  setEntries({
  {"Password", currentPassword.c_str()},
    {"Start"}
  });
}

void WiNetFileManager::update()
{
  if (currentState == STATE_WEB_MANAGER)
  {
    const auto _keeb = &M5Cardputer.Keyboard;
    if (_keeb->isChange() && _keeb->isPressed())
    {
      if (_keeb->isKeyPressed(KEY_BACKSPACE) || _keeb->isKeyPressed('`'))
      {
        currentState = STATE_MENU;
        Template::renderStatus("Quit...");
        HelperUtility::delayMs(1000);
        renderMainMenu();
      }
    }
  } else
  {
    ListScreen::update();
  }
}