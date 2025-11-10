//
// Created by l-sha on 09/10/2025.
//

#include "os/screens/ble/BLEAnalyzerScreen.h"

#include "os/screens/ble/BLEMenuScreen.h"
#include "os/utility/HelperUtility.h"

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
  Template::renderStatus("Scanning...");
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
    entries.push_back({address.toString().c_str()});
  }

  setEntries(entries);
}

void BLEAnalyzerScreen::showInfo()
{
  if (selectedDevice == nullptr)
  {
    Template::renderStatus("Try scan again");
    delayMicroseconds(1500);
    showResult();
    return;
  }

  currentState = STATE_INFO;

  std::string addressType;
  switch (selectedDevice->getAddressType())
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
    {"Name", selectedDevice->getName().c_str()},
    {"Address", selectedDevice->getAddress().toString().c_str()},
    {"Address Type", addressType},
    {"RSSI", std::to_string(selectedDevice->getRSSI())},
    {"Is Connectable", selectedDevice->isConnectable() ? "Yes" : "No"},
    {"Is Scannable", selectedDevice->isScannable() ? "Yes" : "No"},
    {"Service UUIDs", std::to_string(selectedDevice->getServiceUUIDCount())},
    {"Manufacturer Data", std::to_string(selectedDevice->getManufacturerDataCount())},
    {"Service Data", std::to_string(selectedDevice->getServiceDataCount())},
    {"URI", selectedDevice->getURI()},
  });
}

void BLEAnalyzerScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_LIST)
  {
    selectedDevice = scanResults.getDevice(selectedIndex);
    showInfo();
  } else if (currentState == STATE_INFO)
  {
    if (entry.label == "Service UUIDs" && entry.value != "0")
    {
      currentState = STATE_SERVICE_UUID;
      std::vector<ListEntryItem> uuidEntries;
      for (int i = 0; i < selectedDevice->getServiceUUIDCount(); i++)
      {
        uuidEntries.push_back({selectedDevice->getServiceUUID(i).toString().c_str()});
      }

      setEntries(uuidEntries);
    } else if (entry.label == "Manufacturer Data" && entry.value != "0")
    {
      currentState = STATE_MANUFACTURE_DATA;
      std::vector<ListEntryItem> manufactureEntries;
      for (int i = 0; i < selectedDevice->getManufacturerDataCount(); i++)
      {
        manufactureEntries.push_back({
          HelperUtility::stringToHex(selectedDevice->getManufacturerData(i))
        });
      }

      setEntries(manufactureEntries);
    } else if (entry.label == "Service Data" && entry.value != "0")
    {
      currentState = STATE_SERVICE_DATA;
      std::vector<ListEntryItem> serviceDataEntries;
      for (int i = 0; i < selectedDevice->getServiceDataCount(); i++)
      {
        serviceDataEntries.push_back({
          HelperUtility::stringToHex(selectedDevice->getServiceData(i))
        });
      }

      setEntries(serviceDataEntries);
    }
  }
}

void BLEAnalyzerScreen::onEscape()
{
  _global->setScreen(new BLEMenuScreen());
}

void BLEAnalyzerScreen::onBack()
{
  if (currentState == STATE_INFO)
  {
    selectedDevice = nullptr;
    showResult();
  } else if (currentState == STATE_SERVICE_UUID || currentState == STATE_SERVICE_DATA || currentState == STATE_MANUFACTURE_DATA)
  {
    showInfo();
  } else if (currentState == STATE_LIST || currentState == STATE_SCAN)
  {
    onEscape();
  }
}