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
  server.on("/", HTTP_POST, [this]
  {
    if (server.hasArg("ssid"))
    {
      server.send(200, "text/plain", "ssid " + server.arg("ssid") + " uploaded.");
    } else
    {
      server.send(200, "text/plain", "no param");
    }
  });

  server.on("/", HTTP_GET, [this]
  {
    const String html = R"rawliteral(
      <!DOCTYPE html>
      <html>
      <head>
        <title>WiNet File Manager</title>
      </head>
      <body>
        <h1>WiNet File Manager</h1>
        <p>Upload and manage your files here.</p>
        <form method="POST" action="/upload" enctype="multipart/form-data">
          <input type="file" name="file">
          <input type="submit" value="Upload">
        </form>
      </body>
      </html>
    )rawliteral";
    server.send(200, "text/html", html);
  });

  server.onNotFound([this]
  {
    server.send(200, "text/plain", "What are you doing here?");
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