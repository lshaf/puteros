//
// Created by L Shaf on 2025-12-29.
//

#include "ModuleGPSScreen.h"

#include "ModuleMenuScreen.h"
#include "os/component/Template.hpp"
#include "os/screens/MainMenuScreen.hpp"

void ModuleGPSScreen::init()
{
  lastUpdateTime = millis();
  Template::renderHead("Unit GPS v1.1");
  Serial1.begin(115200, SERIAL_8N1, 1);
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
  } else if (currentState == STATE_INFO)
  {
    if (millis() - lastRenderTime > 1000)
    {
      render();
    }

    const auto _kb = &M5Cardputer.Keyboard;
    if (_kb->isChange() && _kb->isPressed())
    {
      if (_kb->isKeyPressed('`') || _kb->isKeyPressed(KEY_BACKSPACE))
      {
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
      //
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
  Serial1.end();
  gps = TinyGPSPlus();
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

void ModuleGPSScreen::renderInfoScreen()
{
  currentState = STATE_INFO;
  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.println(("LAT: " + String(gps.location.lat(), 6)).c_str());
  body.println(("LNG: " + String(gps.location.lng(), 6)).c_str());
  body.println(("SPEED: " + String(gps.speed.kmph(), 2) + " km/h").c_str());
  body.println(("COURSE: " + String(gps.course.deg(), 2) + " deg").c_str());
  body.println(("ALTITUDE: " + String(gps.altitude.meters(), 2) + " m").c_str());
  body.println(("SATELLITES: " + String(gps.satellites.value())).c_str());
  body.println(("DATE: " + String(gps.date.day()) + "/" + String(gps.date.month()) + "/" + String(gps.date.year())).c_str());
  body.println(("TIME (UTC): " + String(gps.time.hour()) + ":" + String(gps.time.minute()) + ":" + String(gps.time.second())).c_str());
  Template::renderBody(&body);
}
