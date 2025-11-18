//
// Created by l-sha on 18/11/2025.
//

#include "BLESpamScreen.h"

#include <esp_gap_ble_api.h>

#include "BLEMenuScreen.h"
#include "os/component/Template.hpp"
#include "os/screens/MainMenuScreen.hpp"

void BLESpamScreen::init()
{
  Template::renderHead("BLE Spam");
  Template::renderStatus("Preparing...");

  BLEDevice::init("PuterOS BLE Spam");
  BLEDevice::startAdvertising();

  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->enableScanResponse(true);
}

void BLESpamScreen::render()
{
  auto body = Template::createBody();

  static int dotCount = 0;
  const std::string loadingStr = "[" + std::string(1, loadingBar[dotCount]) + "] Spamming...";
  dotCount = (dotCount + 1) % 4;

  body.drawCenterString(loadingStr.c_str(), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  body.drawCenterString("esc/back to stop", body.width() / 2, body.height() - body.fontHeight() - 2);
  Template::renderBody(&body);
}

void BLESpamScreen::stopBLE()
{
  pAdvertising->stop();
  esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
}

void BLESpamScreen::update()
{
  if (M5Cardputer.Keyboard.isKeyPressed(KEY_BACKSPACE))
  {
    stopBLE();
    _global->setScreen(new BLEMenuScreen());
    return;
  }

  if (M5Cardputer.Keyboard.isKeyPressed('`'))
  {
    stopBLE();
    _global->setScreen(new MainMenuScreen());
    return;
  }

  const auto now = millis();
  if (now - lastSendTime < 100) return;
  lastSendTime = now;
  pAdvertising->stop();
  delay(5);

  const auto randomizedName = HelperUtility::generateRandomString(15);
  const std::string currentName = randomizedName;
  uint8_t randomAddr[6];
  esp_fill_random(randomAddr, 6);
  randomAddr[0] &= 0x3F;
  BLEDevice::setOwnAddrType(BLE_ADDR_TYPE_RANDOM);
  BLEAddress addr(randomAddr, BLE_ADDR_RANDOM);
  BLEDevice::setOwnAddr(addr);

  BLEAdvertisementData advData;
  advData.setFlags(ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);
  advData.setName(currentName.c_str());

  BLEAdvertisementData scanData;
  scanData.setName(currentName.c_str());

  esp_ble_gap_set_device_name(currentName.c_str());
  pAdvertising->setAdvertisementData(advData);
  pAdvertising->setScanResponseData(scanData);
  pAdvertising->start(0, &addr);
  render();
}