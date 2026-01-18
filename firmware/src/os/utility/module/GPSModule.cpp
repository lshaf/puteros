//
// Created by L Shaf on 2026-01-18.
//

#include "GPSModule.h"

#include <SD.h>
#include <WiFi.h>

void GPSModule::addWigleRecord(const std::string& ssid, const std::string& bssid, const std::string& authMode, const int32_t rssi, const int32_t channel)
{
  const auto fullPath = savePath + "/" + currentFilename;
  auto buffer = SD.open(fullPath.c_str(), FILE_APPEND);
  if (!buffer)
  {
    return;
  }

  const int frequency = channel != 14 ? 2407 + (channel * 5) : 2484;
  buffer.println(
    String(bssid.c_str()) + "," +
    "\"" + String(ssid.c_str()) + "\"," +
    String(authMode.c_str()) + "," +
    getCurrentGPSDate() + " " + getCurrentGPSTime() + "," +
    String(channel) + "," +
    String(frequency) + "," +
    String(rssi) + "," +
    String(gps.location.lat(), 6) + "," +
    String(gps.location.lng(), 6) + "," +
    String(gps.altitude.meters(), 2) + "," +
    String(gps.hdop.hdop() * 1.0) + "," +
    ",,WIFI"
  );
  buffer.close();
}

void GPSModule::doWardrive()
{
  if (wardriveState == WARDRIVE_STATE_IDLE && (millis() - lastWifiScan > 5000))
  {
    WiFi.scanNetworks(true, true);
    wardriveState = WARDRIVE_STATE_SCANNING;
  }

  const int n = WiFi.scanComplete();
  if (n > -1)
  {
    wardriveState = WARDRIVE_STATE_SAVING;
    for (int i = 0; i < n; ++i)
    {
      const bool scanned = isBssidScanned(WiFi.BSSID(i));
      if (scanned) continue;

      String ssid = WiFi.SSID(i);
      String bssid = WiFi.BSSIDstr(i);
      String authMode;
      switch (WiFi.encryptionType(i))
      {
        case WIFI_AUTH_OPEN: authMode = "[OPEN][ESS]"; break;
        case WIFI_AUTH_WEP: authMode = "[WEP][ESS]"; break;
        case WIFI_AUTH_WPA_PSK: authMode = "[WPA-PSK][ESS]"; break;
        case WIFI_AUTH_WPA2_PSK: authMode = "[WPA2-PSK][ESS]"; break;
        case WIFI_AUTH_WPA_WPA2_PSK: authMode = "[WPA-WPA2-PSK][ESS]"; break;
        case WIFI_AUTH_WPA2_ENTERPRISE: authMode = "[WPA2-ENTERPRISE][ESS]"; break;
        default: authMode = "[UNKNOWN]";
      }
      int32_t rssi = WiFi.RSSI(i);
      int32_t channel = WiFi.channel(i);
      addWigleRecord(ssid.c_str(), bssid.c_str(), authMode.c_str(), rssi, channel);
      totalDiscovered++;
    }

    WiFi.scanDelete();
    wardriveState = WARDRIVE_STATE_IDLE;
    lastWifiScan = millis();
  }
}

void GPSModule::endWardrive()
{
  wardriveState = WARDRIVE_STATE_IDLE;
  lastWardriveError = "";
  currentFilename = "";
  totalDiscovered = 0;
  scannedBSSIDs.clear();
  WiFi.mode(WIFI_STA);
}

bool GPSModule::initWardrive()
{
  endWardrive();

  auto cleanDate = String(getCurrentGPSDate());
  auto cleanTime = String(getCurrentGPSTime());
  cleanDate.replace("-", "");
  cleanTime.replace(":", "");
  const String fn = cleanDate + "_" + cleanTime;
  HelperUtility::makeDirectoryRecursive(savePath);
  currentFilename = (fn + ".csv").c_str();

  const auto fullPath = savePath + "/" + currentFilename;
  if (SD.exists(fullPath.c_str()))
  {
    currentFilename = "";
    lastWardriveError = "File " + currentFilename +" already exists!";
    return false;
  }

  auto buffer = SD.open(fullPath.c_str(), FILE_WRITE);
  if (!buffer)
  {
    currentFilename = "";
    lastWardriveError = "Failed to create file!";
    return false;
  }

  buffer.println(
    "WigleWifi-1.6,"
    "appRelease=" + String(APP_VERSION) + ","
    "model=M5Cardputer,release=" + String(APP_VERSION) + ","
    "device=M5Geek,display=M5Cardputer,board=M5Cardputer,brand=M5Geek,star=Sol,body=4,subBody=1"
  );
  buffer.println(
    "MAC,SSID,AuthMode,FirstSeen,Channel,Frequency,RSSI,CurrentLatitude,CurrentLongitude,"
    "AltitudeMeters,AccuracyMeters,RCOIs,MfgrId,Type"
  );
  buffer.close();
  WiFi.mode(WIFI_STA);
  return true;
}
