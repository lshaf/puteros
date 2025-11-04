//
// Created by l-sha on 03/11/2025.
//

#pragma once

#include "NimBLEDevice.h"
#include "NimBLEHIDDevice.h"
#include "Print.h"

#define HID_OFFSET 0x88

typedef uint8_t HidFunction;

const HidFunction HID_FUNCTION_DELETE = 0x4C + HID_OFFSET;
const HidFunction HID_FUNCTION_BACKSPACE = 0x2A + HID_OFFSET;
const HidFunction HID_FUNCTION_TAB = 0x2B + HID_OFFSET;
const HidFunction HID_FUNCTION_ENTER = 0x28 + HID_OFFSET;
const HidFunction HID_FUNCTION_ESC = 0x29 + HID_OFFSET;
const HidFunction HID_FUNCTION_CAPS_LOCK = 0x39 + HID_OFFSET;
const HidFunction HID_FUNCTION_LEFT_CTRL = 0xE0 + HID_OFFSET;
const HidFunction HID_FUNCTION_LEFT_SHIFT = 0xE1 + HID_OFFSET;
const HidFunction HID_FUNCTION_LEFT_ALT = 0xE2 + HID_OFFSET;
const HidFunction HID_FUNCTION_LEFT_GUI = 0xE3 + HID_OFFSET;
const HidFunction HID_FUNCTION_RIGHT_CTRL = 0xE4 + HID_OFFSET;
const HidFunction HID_FUNCTION_RIGHT_SHIFT = 0xE5 + HID_OFFSET;
const HidFunction HID_FUNCTION_RIGHT_ALT = 0xE6 + HID_OFFSET;
const HidFunction HID_FUNCTION_RIGHT_GUI = 0xE7 + HID_OFFSET;
const HidFunction HID_FUNCTION_ARROW_UP = 0x52 + HID_OFFSET;
const HidFunction HID_FUNCTION_ARROW_DOWN = 0x51 + HID_OFFSET;
const HidFunction HID_FUNCTION_ARROW_LEFT = 0x50 + HID_OFFSET;
const HidFunction HID_FUNCTION_ARROW_RIGHT = 0x4F + HID_OFFSET;

typedef uint8_t MediaKeyReport[2];

const MediaKeyReport KEY_MEDIA_NEXT_TRACK = {1, 0};
const MediaKeyReport KEY_MEDIA_PREVIOUS_TRACK = {2, 0};
const MediaKeyReport KEY_MEDIA_STOP = {4, 0};
const MediaKeyReport KEY_MEDIA_PLAY_PAUSE = {8, 0};
const MediaKeyReport KEY_MEDIA_MUTE = {16, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_UP = {32, 0};
const MediaKeyReport KEY_MEDIA_VOLUME_DOWN = {64, 0};
const MediaKeyReport KEY_MEDIA_WWW_HOME = {128, 0};
const MediaKeyReport KEY_MEDIA_LOCAL_MACHINE_BROWSER = {0, 1}; // Opens "My Computer" on Windows
const MediaKeyReport KEY_MEDIA_CALCULATOR = {0, 2};
const MediaKeyReport KEY_MEDIA_WWW_BOOKMARKS = {0, 4};
const MediaKeyReport KEY_MEDIA_WWW_SEARCH = {0, 8};
const MediaKeyReport KEY_MEDIA_WWW_STOP = {0, 16};
const MediaKeyReport KEY_MEDIA_WWW_BACK = {0, 32};
const MediaKeyReport KEY_MEDIA_CONSUMER_CONTROL_CONFIGURATION = {0, 64}; // Media Selection
const MediaKeyReport KEY_MEDIA_EMAIL_READER = {0, 128};

typedef struct
{
  uint8_t modifiers;
  uint8_t reserved;
  uint8_t keys[6];
} KeyReport;

class BLEKeyboardUtility final : public Print, public NimBLEServerCallbacks, public NimBLECharacteristicCallbacks
{
private:
  NimBLEServer* bleServer;
  NimBLEHIDDevice* hid;
  NimBLECharacteristic* inputKeyboard;
  NimBLECharacteristic* outputKeyboard;
  NimBLECharacteristic* inputMedia;
  NimBLEAdvertising* advertising;

  KeyReport _keyReport;
  MediaKeyReport _mediaKeyReport;

  std::string deviceName;
  std::string deviceManufacturer;
  uint8_t batteryLevel;
  uint32_t _delay_ms = 7;
  bool connected = false;
  bool isInitialized = false;

  uint16_t vid = 0xe502;
  uint16_t pid = 0xa111;
  uint16_t version = 0x0210;

public:
  ~BLEKeyboardUtility();
  explicit BLEKeyboardUtility(std::string deviceName = "ESP32 Keyboard", std::string deviceManufacturer = "Espressif", uint8_t batteryLevel = 100);
  void begin();
  void end();
  void sendReport(KeyReport* keys);
  void sendReport(MediaKeyReport* keys);
  size_t press(uint8_t k);
  size_t press(const MediaKeyReport k);
  size_t release(uint8_t k);
  size_t release(const MediaKeyReport k);
  size_t write(uint8_t c) override;
  size_t write(const MediaKeyReport c);
  size_t write(const uint8_t *buffer, size_t size) override;
  void releaseAll();

  void resetPair();

  bool isConnected();
  void setDelay(uint32_t ms)
  {
    this->_delay_ms = ms;
  }
  void setBatteryLevel(uint8_t level)
  {
    this->batteryLevel = level;
    if (hid) hid->setBatteryLevel(level);
  }
  void setName(std::string deviceName)
  {
    this->deviceName = std::move(deviceName);
  }
  void setManufacturer(std::string deviceManufacturer)
  {
    this->deviceManufacturer = std::move(deviceManufacturer);
  }
  void setVendorId(const uint16_t vid)
  {
    this->vid = vid;
  }
  void setProductId(const uint16_t pid)
  {
    this->pid = pid;
  }
  void setVersion(const uint16_t version)
  {
    this->version = version;
  }


  void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
  void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
  void onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo) override;
};
