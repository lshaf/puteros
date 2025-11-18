//
// Created by l-sha on 03/11/2025.
//

#include <NimBLEAdvertising.h>
#include <NimBLEServer.h>
#include "HIDTypes.h"

#include <USBHID.h>
#include <class/hid/hid_device.h>

#include "os/utility/BLEKeyboardUtility.h"
#include "nimble/porting/nimble/include/nimble/nimble_port.h"
#include "os/utility/HelperUtility.h"

#define KEYBOARD_ID 0x01
static constexpr uint8_t report_descriptor[] = {
	TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_REPORT_ID_KEYBOARD))
};

uint8_t USBPutChar(uint8_t c);

BLEKeyboardUtility::~BLEKeyboardUtility()
{
	BLEDevice::deinit(true);
	isInitialized = false;
	connected = false;
}

BLEKeyboardUtility::BLEKeyboardUtility(std::string deviceName, std::string deviceManufacturer, uint8_t batteryLevel)
    : hid(nullptr)
    , deviceName(std::string(std::move(deviceName)).substr(0, 15))
    , batteryLevel(batteryLevel)
{
	this->deviceManufacturer = std::string(std::move(deviceManufacturer)).substr(0,15);
	this->setDelayMs(15);
}

void BLEKeyboardUtility::begin()
{
	if (!isInitialized)
	{
		NimBLEDevice::init(deviceName);
		NimBLEDevice::setSecurityAuth(true, true, true);
		bleServer = NimBLEDevice::createServer();
		bleServer->setCallbacks(this);

		hid = new NimBLEHIDDevice(bleServer);
		inputKeyboard = hid->getInputReport(KEYBOARD_ID);
		outputKeyboard = hid->getOutputReport(KEYBOARD_ID);

		outputKeyboard->setCallbacks(this);

		hid->setBatteryLevel(batteryLevel);
		hid->setManufacturer(deviceManufacturer);
		hid->setPnp(0x02, vid, pid, version);
		hid->setHidInfo(0x00, 0x01);

		const size_t descriptorLength = sizeof(report_descriptor);
		uint8_t finalDescriptor[descriptorLength];
		memcpy(finalDescriptor, report_descriptor, descriptorLength);
		hid->setReportMap(finalDescriptor, descriptorLength);
		hid->startServices();

		isInitialized = true;
	}

  advertising = bleServer->getAdvertising();
  advertising->setAppearance(HID_KEYBOARD);
	advertising->addServiceUUID(hid->getHidService()->getUUID());
  advertising->enableScanResponse(false);
  advertising->start();
}

void BLEKeyboardUtility::end()
{
	const int ret = nimble_port_stop();
	if (ret == 0) {
		nimble_port_deinit();
	}
}

void BLEKeyboardUtility::sendReport(KeyReport* keys)
{
	if (this->connected)
	{
		this->inputKeyboard->setValue(reinterpret_cast<uint8_t*>(keys), sizeof(KeyReport));
		this->inputKeyboard->notify();
		// HelperUtility::delayMs(this->_delay_ms);
	}
}


void BLEKeyboardUtility::resetPair() {
	NimBLEDevice::deleteAllBonds();
}

bool BLEKeyboardUtility::isConnected()
{
	return connected;
}


void BLEKeyboardUtility::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo)
{
	Serial.println("BLEKeyboardUtility::onConnect");
	connected = true;
}

void BLEKeyboardUtility::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason)
{
	Serial.println("BLEKeyboardUtility::onDisconnect");
	connected = false;
}

void BLEKeyboardUtility::onWrite(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo)
{
	auto* value = pCharacteristic->getValue().data();
	(void)value;
}
