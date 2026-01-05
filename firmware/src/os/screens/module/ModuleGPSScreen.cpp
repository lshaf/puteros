//
// Created by L Shaf on 2025-12-29.
//

#include "ModuleGPSScreen.h"

#include <WiFi.h>

#include "ModuleMenuScreen.h"
#include "os/component/InputTextScreen.hpp"
#include "os/component/Template.hpp"
#include "os/screens/MainMenuScreen.hpp"

void ModuleGPSScreen::init()
{
  lastUpdateTime = millis();
  Template::renderHead("Unit GPS v1.1");
  Serial1.begin(115200, SERIAL_8N1, 1);
  render();
}

void ModuleGPSScreen::render()
{
  lastRenderTime = millis();
  if (currentState == STATE_LOADING)
  {
    Template::renderStatus("Waiting for GPS, make sure you go outside to receive GPS Signal...");
  } else if (currentState == STATE_MENU)
  {
    ListScreen::render();
  } else if (currentState == STATE_INFO)
  {
    renderInfoScreen();
  } else if (currentState == STATE_WARDRIVER_ACTION)
  {
    renderWardriverScreen();
  }
}

void ModuleGPSScreen::update()
{
  while (Serial1.available()) gps.encode(Serial1.read());
  if (currentState == STATE_LOADING)
  {
    if (gps.location.isValid())
    {
      renderMenuScreen();
    } else if (millis() - lastUpdateTime > 5000 && gps.charsProcessed() < 10)
    {
      Template::renderStatus("GPS Module not detected! Please check the connection.");
      HelperUtility::delayMs(2000);
      onBack();
    }

    const auto _kb = &M5Cardputer.Keyboard;
    if (_kb->isChange() && _kb->isPressed())
    {
      if (_kb->isKeyPressed('`'))
      {
        onEscape();
      } else if (_kb->isKeyPressed(KEY_BACKSPACE))
      {
        onBack();
      }
    }
  } else if (currentState == STATE_MENU)
  {
    ListScreen::update();
    if (millis() - lastRenderTime > 2000)
    {
      ListScreen::render();
    }
  } else if (currentState == STATE_INFO || currentState == STATE_WARDRIVER_ACTION)
  {
    if (millis() - lastRenderTime > 500)
    {
      render();
    }

    const auto _kb = &M5Cardputer.Keyboard;
    if (_kb->isChange() && _kb->isPressed())
    {
      if (_kb->isKeyPressed('`') || _kb->isKeyPressed(KEY_BACKSPACE))
      {
        if (currentState == STATE_WARDRIVER_ACTION)
        {
          totalDiscovered = 0;
          scannedBSSIDs.clear();
          WiFi.scanDelete();
        }

        renderMenuScreen();
      }
    }
  }
}

void ModuleGPSScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MENU)
  {
    if (entry.label == "View GPS Info")
    {
      renderInfoScreen();
    } else if (entry.label == "Wardriver")
    {

      setEntries({});
      if (!_global->getIsSDCardLoaded())
      {
        Template::renderStatus("SD Card not detected! Please insert an SD Card.");
        HelperUtility::delayMs(1000);
        renderMenuScreen();
        return;
      }

      const auto fn = InputTextScreen::popup("Filename");
      if (fn.empty() || fn.rfind('/') != -1 || fn.rfind('\\') != -1)
      {
        Template::renderStatus("Filename is invalid!", TFT_RED);
        HelperUtility::delayMs(1000);
        renderMenuScreen();
        return;
      }

      HelperUtility::makeDirectoryRecursive(savePath);
      filename = savePath + "/" + fn + ".csv";
      if (SD.exists(filename.c_str()))
      {
        Template::renderStatus(("File " + filename + " already exists!").c_str(), TFT_RED);
        HelperUtility::delayMs(1000);
        renderMenuScreen();
        return;
      }

      auto buffer = SD.open(filename.c_str(), FILE_WRITE);
      if (!buffer)
      {
        Template::renderStatus("Failed to create file!", TFT_RED);
        HelperUtility::delayMs(1000);
        renderMenuScreen();
        return;
      }

      buffer.println(
        "WigleWifi-1.4,"
        "appRelease=" + String(APP_VERSION) + ","
        "model=M5Cardputer,release=" + String(APP_VERSION) + ","
        "device=PuterOS,display=M5Cardputer,board=M5Cardputer,brand=M5Stack"
      );
      buffer.println(
        "MAC,SSID,AuthMode,FirstSeen,Channel,RSSI,CurrentLatitude,"
        "CurrentLongitude,AltitudeMeters,AccuracyMeters,Type"
      );
      buffer.close();
      WiFi.mode(WIFI_STA);
      renderWardriverScreen();
    }
  }
}

void ModuleGPSScreen::onBack()
{
  Template::renderStatus("Stopping GPS Module...");
  endService();
  HelperUtility::delayMs(1000);
  _global->setScreen(new ModuleMenuScreen());
}

void ModuleGPSScreen::onEscape()
{
  Template::renderStatus("Stopping GPS Module...");
  endService();
  HelperUtility::delayMs(1000);
  _global->setScreen(new MainMenuScreen());
}

void ModuleGPSScreen::endService()
{
  Serial1.flush();
  while (Serial1.available()) Serial1.read();
  Serial1.end();
}

void ModuleGPSScreen::preRender(M5Canvas &body)
{
  lastRenderTime = millis();
  const String position = String(gps.location.lat(), 6) + "#" + String(gps.location.lng(), 6);
  body.setTextSize(1.5);
  body.setTextColor(TFT_WHITE);
  body.drawCenterString(position, body.width() / 2, 2);
}

void ModuleGPSScreen::renderMenuScreen()
{
  currentState = STATE_MENU;
  setEntries({
  {"View GPS Info"},
    {"Wardriver"}
  });
}

void ModuleGPSScreen::addWigleRecord(const std::string& ssid, const std::string& bssid, const std::string& authMode, const int32_t rssi, const int32_t channel)
{
  auto buffer = SD.open(filename.c_str(), FILE_APPEND);
  if (!buffer)
  {
    return;
  }

  buffer.println(
    String(bssid.c_str()) + "," +
    String(ssid.c_str()) + "," +
    String(authMode.c_str()) + "," +
    getCurrentGPSDate() + " " + getCurrentGPSTime() + "," +
    String(channel) + "," +
    String(rssi) + "," +
    String(gps.location.lat(), 6) + "," +
    String(gps.location.lng(), 6) + "," +
    String(gps.altitude.meters(), 2) + "," +
    String(gps.hdop.value()) + "," +
    "WIFI"
  );
  buffer.close();
}

void ModuleGPSScreen::renderInfoScreen()
{
  currentState = STATE_INFO;
  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.println(("LAT: " + String(gps.location.lat(), 9)).c_str());
  body.println(("LNG: " + String(gps.location.lng(), 9)).c_str());
  body.println(("SPEED: " + String(gps.speed.kmph(), 2) + " km/h").c_str());
  body.println(("COURSE: " + String(gps.course.deg(), 2) + " deg").c_str());
  body.println(("ALTITUDE: " + String(gps.altitude.meters(), 2) + " m").c_str());
  body.println(("SATELLITES: " + String(gps.satellites.value())).c_str());
  body.println(("DATE: " + getCurrentGPSDate()).c_str());
  body.println(("TIME: " + getCurrentGPSTime() + " UTC").c_str());
  Template::renderBody(&body);
}

void ModuleGPSScreen::renderWardriverScreen()
{
  currentState = STATE_WARDRIVER_ACTION;
  if (!isScanning && (millis() - lastWifiScan > 5000))
  {
    WiFi.scanNetworks(true, true);
    isScanning = true;
  }

  String status = "Idle";
  if (isScanning) status = "Scanning...";
  const int n = WiFi.scanComplete();
  if (n > -1)
  {
    status = String(n) + " networks found";
    for (int i = 0; i < n; ++i)
    {
      bool scanned = isBssidScanned(WiFi.BSSID(i));
      if (scanned) continue;
      String ssid = WiFi.SSID(i);
      String bssid = WiFi.BSSIDstr(i);
      String authMode;
      switch (WiFi.encryptionType(i))
      {
        case WIFI_AUTH_OPEN: authMode = "[OPEN][ESS]"; break;
        case WIFI_AUTH_WEP: authMode = "[WEP][ESS]"; break;
        case WIFI_AUTH_WPA_PSK: authMode = "[WPA-PSK][ESS]"; break;
        case WIFI_AUTH_WPA2_PSK: authMode = "[WPA2-PSK][ESS]"; break;
        case WIFI_AUTH_WPA_WPA2_PSK: authMode = "[WPA-WPA2-PSK][ESS]"; break;
        case WIFI_AUTH_WPA2_ENTERPRISE: authMode = "[WPA2-ENTERPRISE][ESS]"; break;
        default: authMode = "[UNKNOWN]";
      }
      int32_t rssi = WiFi.RSSI(i);
      int32_t channel = WiFi.channel(i);
      addWigleRecord(ssid.c_str(), bssid.c_str(), authMode.c_str(), rssi, channel);
      totalDiscovered++;
    }
    WiFi.scanDelete();
    isScanning = false;
    lastWifiScan = millis();
  }

  Template::renderHead("Wardriving");

  // get csv name from full path
  String csvName = filename.c_str();
  int lastSlash = csvName.lastIndexOf('/');
  if (lastSlash != -1) csvName = csvName.substring(lastSlash + 1);

  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.drawString(getCurrentGPSDate(), 0, 0);
  body.drawString(getCurrentGPSTime() + " UTC", 0, body.fontHeight() + 2);
  body.drawString("File: " + csvName, 0, body.fontHeight() * 2 + 4);
  body.drawRightString(String(gps.location.lat(), 6), body.width(), 0);
  body.drawRightString(String(gps.location.lng(), 6), body.width(), body.fontHeight() + 2);
  body.drawString("Esc to exit", 0, body.height() - body.fontHeight());
  body.drawRightString(status, body.width(), body.height() - body.fontHeight());
  body.drawCenterString("WiFi Discovered", body.width() / 2, body.height() / 2 + body.fontHeight() + 2);
  body.setTextSize(2);
  body.drawCenterString(String(totalDiscovered), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}