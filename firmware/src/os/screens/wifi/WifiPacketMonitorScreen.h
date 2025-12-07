//
// Created by L Shaf on 03/10/25.
//
#pragma once

#include <WiFi.h>
#include <esp_wifi.h>

#include "os/core/ScreenState.hpp"
#include "os/component/Template.hpp"

class WifiPacketMonitorScreen final : public ScreenState
{
private:
  int historySize = 60;
  std::vector<uint16_t> packetHistory;
  unsigned long lastRender = 0;
  int8_t currentChannel = 1;
  void moveChannel(const int8_t direction)
  {
    currentChannel += direction;
    if (currentChannel < 0) currentChannel = 13;
    if (currentChannel > 13) currentChannel = 0;
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    Template::renderHead(("PM Channel " + String(currentChannel)).c_str());
  }

  enum
  {
    STATE_GRAPH,
    STATE_QUIT
  } currentState = STATE_GRAPH;
public:
  void init() override;
  void update() override;
  void render() override;
  void quiting();
  WifiPacketMonitorScreen(): packetHistory(static_cast<size_t>(historySize), 0u) {}
};