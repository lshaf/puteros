//
// Created by L Shaf on 03/10/25.
//

#include <M5Cardputer.h>

#include "os/screens/wifi/WifiPacketMonitorScreen.h"
#include "os/screens/wifi/WifiMenuScreen.h"

static std::atomic<uint32_t> packetCounter{0};
static std::atomic<uint32_t> callbackActive{0};
void WifiPacketMonitorSnifferCallback(void* buf, const wifi_promiscuous_pkt_type_t type)
{
  if (type == WIFI_PKT_MGMT || type == WIFI_PKT_DATA) {
    packetCounter.fetch_add(1, std::memory_order_relaxed);
  }
  callbackActive.fetch_sub(1, std::memory_order_release);
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
  if (currentState == STATE_GRAPH)
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
        quiting();
      }
    }

    if (millis() - lastRender > 1000)
    {
      render();
    }
  }
}

void WifiPacketMonitorScreen::quiting()
{
  currentState = STATE_QUIT;
  Template::renderStatus("Quit...");

  // Unregister callback first to stop new callbacks from being scheduled.
  esp_wifi_set_promiscuous_rx_cb(nullptr);

  // Wait for any in-flight callbacks to finish with a bounded timeout.
  uint32_t waited = 0;
  while (callbackActive.load(std::memory_order_acquire) != 0 && waited < 200) {
    delay(5);
    waited += 5;
  }

  // Now disable promiscuous mode.
  esp_wifi_set_promiscuous(false);

  // Reset local state and release vector memory.
  lastRender = 0;
  currentChannel = 1;
  packetHistory.clear();
  packetHistory.shrink_to_fit();
  HelperUtility::delayMs(500);

  _global->setScreen(new WifiMenuScreen());
}

void WifiPacketMonitorScreen::render()
{
  if (currentState != STATE_GRAPH) return;

  lastRender = millis();
  const uint32_t realCounter = packetCounter.exchange(0, std::memory_order_relaxed);
  auto counter = static_cast<uint16_t>(ceil(realCounter / 2.0));
  if (counter > 90) counter = 90;

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
