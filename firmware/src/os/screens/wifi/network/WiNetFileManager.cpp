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
        _global->getConfig()->set(APP_CONFIG_WIFI_WEB_PASSWORD, newPassword.c_str());
        _global->getConfig()->save();
        currentPassword = newPassword.c_str();
      }

      renderMainMenu();
    }
  }
}

String WiNetFileManager::getContentType(const String& filename){
  if(server.hasArg("download")) return "application/octet-stream";
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

bool WiNetFileManager::isAuthenticated()
{
  if (!server.hasHeader("Cookie")) {
    return false;
  }

  const String cookies = server.header("Cookie");
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
      return true;
    }
  }
  return false;
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
  server.on("/download", HTTP_GET, [this]
  {
    if (!isAuthenticated())
    {
      server.send(401, "text/plain", "not authenticated.");
      return;
    }

    if (!server.hasArg("file"))
    {
      server.send(400, "text/plain", "No file specified.");
      return;
    }

    const auto filePath = server.arg("file");
    if (SD.exists(filePath) == false)
    {
      server.send(404, "text/plain", "File not found.");
      return;
    }

    File file = SD.open(filePath, FILE_READ);
    server.streamFile(file, getContentType(filePath));
    file.close();
  });
  server.on("/upload", HTTP_POST, [this]
  {
    if (!isAuthenticated())
    {
      server.send(401, "text/plain", "not authenticated.");
      return;
    }

    server.send(200, "text/plain", "ok.");
  }, [this]
  {
    if (!isAuthenticated())
    {
      server.send(401, "text/plain", "not authenticated.");
      return;
    }

    const auto upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
      String path = "/";
      if (server.hasArg("folder"))
      {
        path = server.arg("folder");
        if (!path.startsWith("/")) path = "/" + path;
        if (!path.endsWith("/")) path += "/";
      }
      fsUpload = SD.open(path + upload.filename, FILE_WRITE);
    } else if(upload.status == UPLOAD_FILE_WRITE){
      if(fsUpload) fsUpload.write(upload.buf, upload.currentSize);
    } else if(upload.status == UPLOAD_FILE_END){
      if(fsUpload) fsUpload.close();
    }
  });

  server.on("/", HTTP_POST, [this]
  {
    if (!server.hasArg("plain"))
    {
      server.send(404, "text/plain", "404");
      return;
    }

    const auto rawBody = server.arg("plain");
    const auto commands = getBodyCommands(rawBody);
    if (commands[0].isEmpty())
    {
      server.send(200, "text/plain", "No command.");
      return;
    }

    if (commands[0] != "sudo" && !isAuthenticated())
    {
      server.send(401, "text/plain", "not authenticated.");
      return;
    }

    if (commands[0] == "ls")
    {
      String currentPath = "/";
      if (commands.size() >= 2)
      {
        currentPath = commands[1];
      }

      String resp = "";
      File dir = SD.open(currentPath);
      if (!dir || !dir.isDirectory())
      {
        server.send(403, "text/plain", "Not a directory.");
        return;
      }

      File file = dir.openNextFile();
      while (file)
      {
        resp += (file.isDirectory() ? "DIR:" : "FILE:") + String(file.name()) + ":" + String(file.size()) + "\n";
        file = dir.openNextFile();
      }

      server.send(200, "text/plain", resp);
    } else if (commands[0] == "rm")
    {
      if (commands.size() < 2)
      {
        server.send(400, "text/plain", "No file specified.");
        return;
      }

      const auto filePath = commands[1];
      if (SD.exists(filePath) == false)
      {
        server.send(404, "text/plain", "File not found.");
        return;
      }

      if (SD.remove(filePath))
      {
        server.send(200, "text/plain", "File deleted.");
      } else
      {
        server.send(500, "text/plain", "Failed to delete file.");
      }
    } else if (commands[0] == "mv")
    {
      if (commands.size() < 3)
      {
        server.send(400, "text/plain", "Source or destination not specified.");
        return;
      }

      const auto sourcePath = commands[1];
      const auto destPath = commands[2];
      if (SD.exists(sourcePath) == false)
      {
        server.send(404, "text/plain", "Source file not found.");
        return;
      }

      if (SD.rename(sourcePath, destPath))
      {
        server.send(200, "text/plain", "File moved/renamed.");
      } else
      {
        server.send(500, "text/plain", "Failed to move/rename file.");
      }
    } else if (commands[0] == "mkdir")
    {
      if (commands.size() < 2)
      {
        server.send(400, "text/plain", "No directory name specified.");
        return;
      }

      const auto dirPath = commands[1];
      if (SD.mkdir(dirPath))
      {
        server.send(200, "text/plain", "Directory created.");
      } else
      {
        server.send(500, "text/plain", "Failed to create directory.");
      }
    } else if (commands[0] == "touch")
    {
      if (commands.size() < 2)
      {
        server.send(400, "text/plain", "No file name specified.");
        return;
      }

      const auto filePath = commands[1];
      File file = SD.open(filePath, FILE_WRITE);
      if (file)
      {
        file.close();
        server.send(200, "text/plain", "File created.");
      } else
      {
        server.send(500, "text/plain", "Failed to create file.");
      }
    } else if (commands[0] == "cat")
    {
      if (commands.size() < 2)
      {
        server.send(400, "text/plain", "No file specified.");
        return;
      }

      const auto filePath = commands[1];
      if (SD.exists(filePath) == false)
      {
        server.send(404, "text/plain", "File not found.");
        return;
      }

      File file = SD.open(filePath);
      String resp = "";
      while (file.available())
      {
        resp += char(file.read());
      }
      file.close();
      server.send(200, "text/plain", resp);
    } else if (commands[0] == "echo")
    {
      if (commands.size() < 3)
      {
        server.send(400, "text/plain", "No file or content specified.");
        return;
      }

      const auto filePath = commands[1];
      File file = SD.open(filePath, FILE_WRITE);
      if (!file)
      {
        server.send(500, "text/plain", "Failed to open file.");
        return;
      }

      // Write all lines after the filepath (index 2 onwards)
      for (size_t i = 2; i < commands.size(); ++i)
      {
        if (i > 2) file.print('\n');
        file.print(commands[i]);
      }

      file.close();
      server.send(200, "text/plain", "Content written to file.");
    }else if (commands[0] == "sysinfo")
    {
      String resp = "PuterOS File Manager\n";
      resp += "FS:" + String(SD.totalBytes() - SD.usedBytes()) + "\n";
      resp += "US:" + String(SD.usedBytes()) + "\n";
      resp += "TS:" + String(SD.totalBytes()) + "\n";
      server.send(200, "text/plain", resp);
    } else if (commands[0] == "sudo")
    {
      if (commands.size() < 2)
      {
        server.send(400, "text/plain", "No password specified.");
        return;
      }

      const auto password = commands[1];
      if (password == currentPassword)
      {
        const auto sessionToken = String(random(0x7FFFFFFF), HEX);

        // Set HTTPOnly cookie (Max-Age in seconds)
        server.sendHeader("Set-Cookie", "session=" + sessionToken + "; HttpOnly; Max-Age=3600; Path=/");
        server.send(200, "text/plain", "Login successful");

        // Store session token for validation
        SESSION_COUNTER = (SESSION_COUNTER + 1) % MAX_SESSIONS;
        activeSessions[SESSION_COUNTER] = sessionToken;
        server.send(200, "text/plain", "ok");
      } else
      {
        server.send(403, "text/plain", "forbidden");
      }
    } else
    {
      server.send(404, "text/plain", "command not found");
    }
  });

  server.onNotFound([this]
  {
    server.send(404, "text/plain", "404");
  });

  server.on("/", [this]
  {
    server.serveStatic("/", SD, "/puteros/web/file_manager/");
  });

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
    server.handleClient();
    const auto _keeb = &M5Cardputer.Keyboard;
    if (_keeb->isChange() && _keeb->isPressed())
    {
      if (_keeb->isKeyPressed(KEY_BACKSPACE) || _keeb->isKeyPressed('`'))
      {
        currentState = STATE_MENU;
        Template::renderStatus("Quit...");
        server.stop();
        HelperUtility::delayMs(1000);
        renderMainMenu();
      }
    }
  } else
  {
    ListScreen::update();
  }
}