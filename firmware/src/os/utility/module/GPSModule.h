//
// Created by L Shaf on 2026-01-18.
//

#pragma once
#include "TinyGPS++.h"
#include "os/utility/CircularArray.h"
#include "os/utility/HelperUtility.h"

class GPSModule
{
public:
  TinyGPSPlus gps;
  enum WARDRIVE_STATE_E
  {
    WARDRIVE_STATE_IDLE,
    WARDRIVE_STATE_SCANNING,
    WARDRIVE_STATE_SAVING
  } wardriveState = WARDRIVE_STATE_IDLE;

  void begin(HardwareSerial &serialPort, uint32_t baudRate = 115200, const int8_t rxPin = 1)
  {
    serial = &serialPort;
    serial->begin(baudRate, SERIAL_8N1, rxPin);
  }

  void end()
  {
    if (serial != nullptr)
    {
      serial->flush();
      while (serial->available()) serial->read();
      serial->end();
      serial = nullptr;
    }
  }

  String getCurrentGPSDate()
  {
    return String(gps.date.year()) + "-" + HelperUtility::padNumber(gps.date.month(), 2) + "-" + HelperUtility::padNumber(gps.date.day(), 2);
  }

  String getCurrentGPSTime()
  {
    return HelperUtility::padNumber(gps.time.hour(), 2) + ":" + HelperUtility::padNumber(gps.time.minute(), 2) + ":" + HelperUtility::padNumber(gps.time.second(), 2);
  }

  void update()
  {
    if (serial == nullptr) return;
    while (serial->available()) gps.encode(serial->read());
  }

  std::string getCurrentFilename() { return currentFilename; }
  std::string getLastWardriveError() { return lastWardriveError; }
  int16_t getDiscoveredWifi() { return totalDiscovered; }
  WARDRIVE_STATE_E statusWardrive() { return wardriveState; }
  void doWardrive();
  bool initWardrive();
  void endWardrive();

private:
  HardwareSerial *serial = nullptr;
  std::string savePath = "/puteros/gps/wardriver";
  std::string currentFilename = "";

  uint16_t totalDiscovered = 0;
  unsigned long lastWifiScan = 0;
  std::string lastWardriveError = "";

  void addWigleRecord(
    const std::string& ssid, const std::string& bssid, const std::string& authMode,
    int32_t rssi, int32_t channel
  );

  using BSSID = std::array<uint8_t, 6>;
  CircularBuffer<BSSID> scannedBSSIDs{80};

  bool isBssidScanned(const uint8_t* bssid)
  {
    BSSID bssidArray;
    std::copy(bssid, bssid + 6, bssidArray.begin());
    for (int i = 0; i < scannedBSSIDs.size(); ++i)
    {
      const auto& scanned = scannedBSSIDs.get(i);
      if (scanned == bssidArray)
      {
        return true;
      }
    }
    scannedBSSIDs.push(bssidArray);
    return false;
  }
};
