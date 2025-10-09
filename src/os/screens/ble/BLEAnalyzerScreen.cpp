//
// Created by l-sha on 09/10/2025.
//

#include "os/screens/ble/BLEAnalyzerScreen.h"

#include "os/screens/ble/BLEMenuScreen.h"

void BLEAnalyzerScreen::init()
{
  NimBLEDevice::init("");
  bleScan = NimBLEDevice::getScan();

  Template::renderHead("BLE Analyzer");
  startScan();
}

void BLEAnalyzerScreen::startScan()
{
  currentState = STATE_SCAN;
  setEntries({});
  Template::drawStatusBody("Scanning...");
  bleScan->clearResults();
  scanResults = bleScan->getResults(5000, false);

  showResult();
}

void BLEAnalyzerScreen::showResult()
{
  currentState = STATE_LIST;
  entries.clear();
  for (int i = 0; i < scanResults.getCount(); i++)
  {
    const NimBLEAdvertisedDevice* device = scanResults.getDevice(i);
    const auto address = device->getAddress();
    entries.push_back({
      address.toString().c_str(),
        std::to_string(i)
    });
  }

  setEntries(entries);
}

void BLEAnalyzerScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_LIST)
  {
    const NimBLEAdvertisedDevice* device = scanResults.getDevice(std::stoi(entry.value));
    if (device == nullptr)
    {
      Template::drawStatusBody("Try scan again");
      delayMicroseconds(1500);
      showResult();
      return;
    }

    currentState = STATE_INFO;

    std::string addressType;
    switch (device->getAddressType())
    {
    case BLE_ADDR_PUBLIC:
      addressType = "Public";
      break;
    case BLE_ADDR_RANDOM:
      addressType = "Random";
      break;
    case BLE_ADDR_PUBLIC_ID:
      addressType = "Public ID";
      break;
    case BLE_ADDR_RANDOM_ID:
      addressType = "Random ID";
      break;
    default:
      addressType = "Unknown";
      break;
    }

    setEntries({
      {"Name", device->getName().c_str()},
      {"Address", device->getAddress().toString().c_str()},
      {"Address Type", addressType},
      {"RSSI", std::to_string(device->getRSSI())},
      {"Is Connectable", device->isConnectable() ? "Yes" : "No"},
      {"Is Scannable", device->isScannable() ? "Yes" : "No"},
      {"Service UUIDs", std::to_string(device->getServiceUUIDCount())},
      {"Manufacturer Data", std::to_string(device->getManufacturerDataCount())},
      {"Service Data", std::to_string(device->getServiceDataCount())},
      {"URI", device->getURI()},
    });
  }
}

void BLEAnalyzerScreen::onEscape()
{
  stopScan();
  _global->setScreen(new BLEMenuScreen());
}

void BLEAnalyzerScreen::onBack()
{
  if (currentState == STATE_INFO)
  {
    showResult();
  } else if (currentState == STATE_LIST || currentState == STATE_SCAN)
  {
    onEscape();
  }
}