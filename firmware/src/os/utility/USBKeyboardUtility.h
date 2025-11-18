//
// Created by l-sha on 18/11/2025.
//

#pragma once

#include <USBHID.h>
#include "HIDKeyboardUtility.h"

ESP_EVENT_DECLARE_BASE(ARDUINO_USB_HID_KEYBOARD_EVENTS);

typedef enum {
  ARDUINO_USB_HID_KEYBOARD_ANY_EVENT = ESP_EVENT_ANY_ID,
  ARDUINO_USB_HID_KEYBOARD_LED_EVENT = 0,
  ARDUINO_USB_HID_KEYBOARD_MAX_EVENT,
} arduino_usb_hid_keyboard_event_t;

typedef union {
  struct {
    uint8_t numlock:1;
    uint8_t capslock:1;
    uint8_t scrolllock:1;
    uint8_t compose:1;
    uint8_t kana:1;
    uint8_t reserved:3;
  };
  uint8_t leds;
} arduino_usb_hid_keyboard_event_data_t;

class USBKeyboardUtility final : public USBHIDDevice, public HIDKeyboardUtility
{
public:
  USBKeyboardUtility();
  void begin() override;
  void end() override;
  void sendReport(KeyReport* keys) override;

  void onEvent(esp_event_handler_t callback);
  void onEvent(arduino_usb_hid_keyboard_event_t event, esp_event_handler_t callback);

  // internal use
  uint16_t _onGetDescriptor(uint8_t* buffer) override;
  void _onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len) override;

private:
  USBHID hid;
};
