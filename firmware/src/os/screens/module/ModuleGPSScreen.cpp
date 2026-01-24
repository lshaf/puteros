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
  gps_module.begin(&Serial1, 115200, 1);
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
  gps_module.update();
  if (currentState == STATE_MENU && millis() - lastRenderTime > 2000) ListScreen::render();
  if (currentState == STATE_INFO || currentState == STATE_WARDRIVER_ACTION)
  {
    if (currentState == STATE_WARDRIVER_ACTION) gps_module.doWardrive();
    if (millis() - lastRenderTime > 500) render();
  }

  if (currentState == STATE_LOADING)
  {
    if (gps_module.gps.location.isValid())
    {
      renderMenuScreen();
    } else if (millis() - lastUpdateTime > 5000 && gps_module.gps.charsProcessed() < 10)
    {
      Template::renderStatus("GPS Module not detected! Please check the connection.");
      HelperUtility::delayMs(2000);
      onBack();
    }
  }

  ListScreen::update();
}

void ModuleGPSScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MENU)
  {
    if (entry.label == "View GPS Info")
    {
      currentState = STATE_INFO;
      renderInfoScreen();
    } else if (entry.label == "Wardriver")
    {
      setEntries({});
      if (gps_module.initWardrive() == false)
      {
        Template::renderStatus("Wardrive Error: " + gps_module.getLastWardriveError(), TFT_RED);
        HelperUtility::delayMs(2000);
        currentState = STATE_MENU;
        render();
        return;
      }

      Template::renderHead("Wardriving");
      currentState = STATE_WARDRIVER_ACTION;
      renderWardriverScreen();
    }
  }
}

void ModuleGPSScreen::onBack()
{
  if (currentState == STATE_MENU || currentState == STATE_LOADING)
  {
    Template::renderStatus("Stopping GPS Module...");
    gps_module.end();
    HelperUtility::delayMs(1000);
    _global->setScreen(new ModuleMenuScreen());
  } else
  {
    if (currentState == STATE_WARDRIVER_ACTION) gps_module.endWardrive();
    currentState = STATE_MENU;
    renderMenuScreen();
  }
}

void ModuleGPSScreen::onEscape()
{
  if (currentState == STATE_WARDRIVER_ACTION) gps_module.endWardrive();
  Template::renderStatus("Stopping GPS Module...");
  gps_module.end();
  HelperUtility::delayMs(1000);
  _global->setScreen(new MainMenuScreen());
}

void ModuleGPSScreen::preRender(M5Canvas &body)
{
  if (currentState == STATE_MENU)
  {
    lastRenderTime = millis();
    auto gps = gps_module.gps;
    const String position = String(gps.location.lat(), 6) + "#" + String(gps.location.lng(), 6);
    body.setTextSize(1.5);
    body.setTextColor(TFT_WHITE);
    body.drawCenterString(position, body.width() / 2, 2);
  }
}

void ModuleGPSScreen::renderMenuScreen()
{
  currentState = STATE_MENU;
  setEntries({
  {"View GPS Info"},
    {"Wardriver"}
  });
}

void ModuleGPSScreen::renderInfoScreen()
{
  auto gps = gps_module.gps;
  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.println(("LAT: " + String(gps.location.lat(), 9)).c_str());
  body.println(("LNG: " + String(gps.location.lng(), 9)).c_str());
  body.println(("SPEED: " + String(gps.speed.kmph(), 2) + " km/h").c_str());
  body.println(("COURSE: " + String(gps.course.deg(), 2) + " deg").c_str());
  body.println(("ALTITUDE: " + String(gps.altitude.meters(), 2) + " m").c_str());
  body.println(("SATELLITES: " + String(gps.satellites.value())).c_str());
  body.println(("DATE: " + gps_module.getCurrentGPSDate()).c_str());
  body.println(("TIME: " + gps_module.getCurrentGPSTime() + " UTC").c_str());
  Template::renderBody(&body);
}

void ModuleGPSScreen::renderWardriverScreen()
{
  // get csv name from full path
  String csvName = gps_module.getCurrentFilename().c_str();

  auto gps = gps_module.gps;
  String status;
  switch (gps_module.statusWardrive())
  {
    case GPSModule::WARDRIVE_STATE_IDLE:
      status = "Idle";
      break;
    case GPSModule::WARDRIVE_STATE_SCANNING:
      status = "Scanning...";
      break;
    case GPSModule::WARDRIVE_STATE_SAVING:
      status = "Saving...";
      break;
    default:
      status = "Unknown";
      break;
  }
  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.drawString(gps_module.getCurrentGPSDate(), 0, 0);
  body.drawString(gps_module.getCurrentGPSTime() + " UTC", 0, body.fontHeight() + 2);
  body.drawString("File: " + csvName, 0, body.fontHeight() * 2 + 4);
  body.drawRightString(String(gps.location.lat(), 6), body.width(), 0);
  body.drawRightString(String(gps.location.lng(), 6), body.width(), body.fontHeight() + 2);
  body.drawString("Esc to exit", 0, body.height() - body.fontHeight());
  body.drawRightString(status, body.width(), body.height() - body.fontHeight());
  body.drawCenterString("WiFi Discovered", body.width() / 2, body.height() / 2 + body.fontHeight() + 2);
  body.setTextSize(2);
  body.drawCenterString(String(gps_module.getDiscoveredWifi()), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}