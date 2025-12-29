//
// Created by l-sha on 9/23/2025.
//

#include <WiFi.h>

#include "WifiNetworkScreen.h"
#include "WiNetInformationScreen.h"

void WiNetInformationScreen::init()
{
  Template::renderHead("NET Info");
  Template::renderStatus("Fetching Info...");
  render();
}

void WiNetInformationScreen::update()
{
  const auto key = &M5Cardputer.Keyboard;
  if (key->isChange() && key->isPressed())
  {
    if (key->isKeyPressed(KEY_BACKSPACE) || key->isKeyPressed('`'))
    {
      _global->setScreen(new WifiNetworkScreen());
    }
  }
}

void WiNetInformationScreen::render()
{
  auto body = Template::createBody();

  body.drawRightString(WiFi.localIP().toString().c_str(), body.width() - 3, body.getCursorY());
  body.println("IP");

  body.drawRightString(WiFi.dnsIP().toString().c_str(), body.width() - 3, body.getCursorY());
  body.println("DNS IP");

  body.drawRightString(WiFi.gatewayIP().toString().c_str(), body.width() - 3, body.getCursorY());
  body.println("Gateway");

  body.drawRightString(WiFi.subnetMask().toString().c_str(), body.width() - 3, body.getCursorY());
  body.println("Subnet");
  body.println();


  body.drawRightString(String(WiFi.channel()), body.width() - 3, body.getCursorY());
  body.println("Channel");

  body.drawRightString(WiFi.SSID().c_str(), body.width() - 3, body.getCursorY());
  body.println("SSID");

  body.drawRightString(String(WiFi.RSSI()) + " dBm", body.width() - 3, body.getCursorY());
  body.println("RSSI");

  body.drawRightString(WiFi.getHostname(), body.width() - 3, body.getCursorY());
  body.println("Hostname");

  body.drawRightString(WiFi.macAddress().c_str(), body.width() - 3, body.getCursorY());
  body.println("MAC");

  body.drawRightString(WiFi.BSSIDstr().c_str(), body.width() - 3, body.getCursorY());
  body.println("BSSID");

  Template::renderBody(&body);
}