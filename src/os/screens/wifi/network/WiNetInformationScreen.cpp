//
// Created by l-sha on 9/23/2025.
//

#include <WiFi.h>

#include "os/screens/wifi/WifiNetworkScreen.h"
#include "os/screens/wifi/network/WiNetInformationScreen.h"

void WiNetInformationScreen::init()
{
  Template::renderHead("NET Info");
  Template::drawStatusBody("Fetching Info...");
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
  body.print("IP:");
  body.println(WiFi.localIP().toString().c_str());

  body.print("DNS IP:");
  body.println(WiFi.dnsIP().toString().c_str());

  body.print("Gateway IP:");
  body.println(WiFi.gatewayIP().toString().c_str());

  body.print("Subnet Mask:");
  body.println(WiFi.subnetMask().toString().c_str());
  body.println();

  body.print("SSID:");
  body.println(WiFi.SSID().c_str());

  body.print("RSSI: ");
  body.print(WiFi.RSSI());
  body.println(" dBm");

  body.print("Hostname:");
  body.println(WiFi.getHostname());

  body.print("MAC Address:");
  body.println(WiFi.macAddress().c_str());

  body.print("BSSID:");
  body.println(WiFi.BSSIDstr().c_str());

  body.print("Channel: ");
  body.println(WiFi.channel());

  Template::renderBody(&body);
}