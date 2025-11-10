//
// Created by L Shaf on 03/10/25.
//

#include <M5Cardputer.h>

#include "os/screens/wifi/WifiPacketMonitorScreen.h"
#include "os/screens/wifi/WifiMenuScreen.h"

volatile uint32_t packetCounter = 0;
void WifiPacketMonitorSnifferCallback(void* buf, const wifi_promiscuous_pkt_type_t type)
{
  if (type == WIFI_PKT_MGMT || type == WIFI_PKT_DATA) {
    packetCounter++;
  }
}

void WifiPacketMonitorScreen::init()
{
  currentChannel = 1;
  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&WifiPacketMonitorSnifferCallback);
  moveChannel(0);
  render();
}

void WifiPacketMonitorScreen::update()
{
  const auto _k = &M5Cardputer.Keyboard;
  if (_k->isChange() && _k->isPressed())
  {
    if (_k->isKeyPressed(';'))
    {
      moveChannel(1);
    } else if (_k->isKeyPressed('.'))
    {
      moveChannel(-1);
    } else if (_k->isKeyPressed('`'))
    {
      _global->setScreen(new WifiMenuScreen());
    }
  }

  if (millis() - lastRender > 1000)
  {
    render();
  }
}

void WifiPacketMonitorScreen::render()
{
  lastRender = millis();
  const auto realCounter = packetCounter;
  auto counter = realCounter / 2.0;
  if (counter > 90) counter = 90;
  packetCounter = 0;

  auto body = Template::createBody();
  for (int i = historySize - 1; i >= 0; --i) {
    if (i == 0)
    {
      packetHistory[0] = counter;
    } else
    {
      packetHistory[i] = packetHistory[i - 1];
    }

    body.fillRect(i * 4, body.height() - 10 - packetHistory[i], 3, packetHistory[i], TFT_BLUE);
  }

  body.setTextColor(TFT_WHITE, TFT_BLACK);
  body.drawString(String(realCounter) + " pkt/s", 0, 0);
  body.drawString("ESC: Exit | UP/DOWN: Change Channel", 0, body.height() - body.fontHeight());

  Template::renderBody(&body);
}
