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
  uint32_t* packetHistory = new uint32_t[historySize]();
  unsigned long lastRender = 0;
  unsigned int currentChannel = 1;
  void moveChannel(const int direction)
  {
    currentChannel += direction;
    if (currentChannel < 1) currentChannel = 13;
    if (currentChannel > 13) currentChannel = 1;
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    Template::renderHead(("PM Channel " + String(currentChannel)).c_str());
  }
public:
  void init() override;
  void update() override;
  void render() override;
  ~WifiPacketMonitorScreen() override
  {
    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr);
    delete[] packetHistory;
    packetHistory = nullptr;
    lastRender = 0;
    currentChannel = 1;
    historySize = 0;
  }
};