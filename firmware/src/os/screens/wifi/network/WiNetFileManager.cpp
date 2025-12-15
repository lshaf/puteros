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

  Template::renderStatus("Starting Server...");
  server.on("/upload", HTTP_POST, [this]
  {
    server.send(200, "text/plain", "ok.");
  }, [this]
  {
    if (!_global->getIsSDCardLoaded())
    {
      server.send(403, "text/plain", "SD Card not loaded.");
      return;
    }

    const auto upload = server.upload();
    if (upload.status == UPLOAD_FILE_START)
    {
      fsUpload = SD.open("/" + upload.filename, FILE_WRITE);
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

    server.send(200, "text/plain", "ssid " + server.arg("plain") + " uploaded.");
  });

  server.onNotFound([this]
  {
    server.send(404, "text/plain", "404");
  });
  
  server.serveStatic("/", SD, "/puteros/web/file_manager");

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