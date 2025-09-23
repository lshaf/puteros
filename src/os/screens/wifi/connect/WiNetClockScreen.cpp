//
// Created by l-sha on 9/23/2025.
//

#include <WiFi.h>
#include <time.h>

#include "os/component/Template.hpp"
#include "os/screens/wifi/connect/WiNetClockScreen.h"

#include "os/screens/wifi/WifiNetworkScreen.h"
#include "os/screens/wifi/WifiMenuScreen.h"

void WifiConnectClockScreen::init()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    _global->setScreen(new WifiMenuScreen());
  }

  Template::renderHead("Clock");
  Template::drawStatusBody("Connecting to Server");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
}

void WifiConnectClockScreen::update()
{
  const auto key = &M5Cardputer.Keyboard;
  if (timeToRefresh())
  {
    lastRenderTime = millis();
    render();
  }

  if (key->isChange() && key->isPressed())
  {
    if (key->isKeyPressed(';'))
    {
      offsetMinutes += 30;
      render();
    } else if (key->isKeyPressed('.'))
    {
      offsetMinutes -= 30;
      render();
    } else if (key->isKeyPressed(KEY_BACKSPACE) || key->isKeyPressed('`'))
    {
      _global->setScreen(new WifiNetworkScreen());
    }
  }
}

void WifiConnectClockScreen::render()
{
  Template::renderHead("Clock");

  struct tm timeInfo;
  if (!getLocalTime(&timeInfo))
  {
    Template::drawStatusBody("Retrying...");
    return;
  }

  constexpr int MIN_OFFSET = -12 * 60;
  constexpr int MAX_OFFSET = 14 * 60;

  if (offsetMinutes > MAX_OFFSET)
    offsetMinutes = MIN_OFFSET;
  else if (offsetMinutes < MIN_OFFSET)
    offsetMinutes = MAX_OFFSET;

  timeInfo.tm_min += offsetMinutes;
  mktime(&timeInfo);

  char timeString[16];
  strftime(timeString, sizeof(timeString), "%H:%M:%S", &timeInfo);
  auto body = Template::createBody();
  body.setTextSize(1);
  char offsetStr[16];
  snprintf(offsetStr, sizeof(offsetStr), "UTC %+d:%02d", offsetMinutes / 60, abs(offsetMinutes % 60));
  body.drawCenterString(offsetStr, body.width() / 2, body.height() / 2 - body.fontHeight() * 2 - 1);
  body.setTextSize(2);
  body.drawCenterString(timeString, body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}