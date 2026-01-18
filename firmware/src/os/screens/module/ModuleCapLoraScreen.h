//
// Created by L Shaf on 2026-01-17.
//

#pragma once

#include "os/core/ListScreen.hpp"
#include "os/utility/module/GPSModule.h"
#include <RadioLib.h>

#define LORA_BW           125.0f // SX1262 bandwidth (kHz) - 125kHz is common for long-range
#define LORA_SF           12     // Spreading factor (6-12) - higher = longer range, slower data rate
#define LORA_CR           5      // Coding rate (5-8, represents 4/5 to 4/8) - higher = more error correction
#define LORA_FREQ         868.0  // Carrier frequency (MHz) - 868MHz is EU ISM band for SX1262
#define LORA_SYNC_WORD    0x12   // Sync word for packet recognition - must match between transmitter/receiver
// define LORA_SYNC_WORD    RADIOLIB_SX126X_SYNC_WORD_PRIVATE   // Sync word for packet recognition - must match between transmitter/receiver
#define LORA_TX_POWER     22     // Transmission power (dBm) - 22dBm is maximum for many regions
#define LORA_PREAMBLE_LEN 20     // Preamble length (symbols) - ensures receiver can detect packet start

class ModuleCapLoraScreen final : public ListScreen
{
protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;

  void initChat();
  void showChatScreen();
  void showMainMenuScreen();
  void showGSPDataScreen();
  void showWardriveScreen();

public:
  void init() override;
  void update() override;
  void render() override;
  bool isAutoPowerOff() override { return false; }

private:
  GPSModule gps_module;
  unsigned long lastRefreshTime = 0;
  SX1262 lora = new Module(LORA_SPI_CS_PIN, LORA_IRQ_PIN, LORA_RESET_PIN, LORA_BUSY_PIN);
  enum
  {
    STATE_MENU,
    STATE_CHAT_SETTING,
    STATE_GPS_DATA,
    STATE_WARDRIVING,
  } currentState = STATE_MENU;

  bool isLoraAttached();
};
