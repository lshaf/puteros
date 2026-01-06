//
// Created by l-sha on 05/11/2025.
//

#pragma once

#include <cstring>
#include <WiFi.h>
#include <esp_wifi.h>
#include <unordered_map>

#include "os/core/ListScreen.hpp"

class WifiDeauthDetectorScreen final : public ListScreen
{
public:
  bool isAutoPowerOff() override {return false;}
  ~WifiDeauthDetectorScreen() override
  {
    esp_wifi_set_promiscuous(false);
    esp_wifi_set_promiscuous_rx_cb(nullptr);
    deauthDetectedList.clear();
    ssidList.clear();
  }

  void init() override;
  void update() override;

  typedef std::array<uint8_t, 6> MacAddr;
  struct deauthPacketData
  {
    std::string ssid;
    unsigned long timestamp;
    int counter;
  };

  struct MacHash {
    size_t operator()(MacAddr const& m) const noexcept {
      uint64_t v = 0;
      std::memcpy(&v, m.data(), 6);
      return std::hash<uint64_t>{}(v);
    }
  };

  struct MacEqual {
    bool operator()(MacAddr const& a, MacAddr const& b) const noexcept {
      return std::memcmp(a.data(), b.data(), 6) == 0;
    }
  };


  static std::unordered_map<MacAddr, std::string, MacHash, MacEqual> ssidList;
  static std::unordered_map<MacAddr, deauthPacketData, MacHash, MacEqual> deauthDetectedList;
  static void deauthDetectedCallback(void* buf, wifi_promiscuous_pkt_type_t type);

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;

private:
  enum State_e
  {
    STATE_LISTED,
    STATE_NO_LIST
  } currentState = STATE_LISTED;
  int currentChannel = 1;
  unsigned long lastRender = 0;
  static constexpr unsigned long windowMs = 30000UL; // 10 seconds

  static portMUX_TYPE s_deauthLock;
  static portMUX_TYPE s_ssidLock;
};
