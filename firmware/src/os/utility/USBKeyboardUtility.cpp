//
// Created by l-sha on 18/11/2025.
//

#include <USB.h>
#include "USBKeyboardUtility.h"

ESP_EVENT_DEFINE_BASE(ARDUINO_USB_HID_KEYBOARD_EVENTS);
esp_err_t arduino_usb_event_post(esp_event_base_t event_base, int32_t event_id, void *event_data, size_t event_data_size, TickType_t ticks_to_wait);
esp_err_t arduino_usb_event_handler_register_with(esp_event_base_t event_base, int32_t event_id, esp_event_handler_t event_handler, void *event_handler_arg);

static constexpr uint8_t report_descriptor[] = {
    TUD_HID_REPORT_DESC_KEYBOARD(HID_REPORT_ID(HID_REPORT_ID_KEYBOARD))
};

USBKeyboardUtility::USBKeyboardUtility()
{
    static bool initialized = false;
    if(!initialized){
        initialized = true;
        hid.addDevice(this, sizeof(report_descriptor));
        this->setDelayMs(3);
    }
}

void USBKeyboardUtility::onEvent(esp_event_handler_t callback){
    onEvent(ARDUINO_USB_HID_KEYBOARD_ANY_EVENT, callback);
}
void USBKeyboardUtility::onEvent(arduino_usb_hid_keyboard_event_t event, esp_event_handler_t callback){
    arduino_usb_event_handler_register_with(ARDUINO_USB_HID_KEYBOARD_EVENTS, event, callback, this);
}
void USBKeyboardUtility::_onOutput(uint8_t report_id, const uint8_t* buffer, uint16_t len){
    if(report_id == HID_REPORT_ID_KEYBOARD){
        arduino_usb_hid_keyboard_event_data_t p;
        p.leds = buffer[0];
        arduino_usb_event_post(ARDUINO_USB_HID_KEYBOARD_EVENTS, ARDUINO_USB_HID_KEYBOARD_LED_EVENT, &p, sizeof(arduino_usb_hid_keyboard_event_data_t), portMAX_DELAY);
    }
}
uint16_t USBKeyboardUtility::_onGetDescriptor(uint8_t* buffer){
    memcpy(buffer, report_descriptor, sizeof(report_descriptor));
    return sizeof(report_descriptor);
}

void USBKeyboardUtility::begin()
{
    USB.begin();
    hid.begin();
}

void USBKeyboardUtility::sendReport(KeyReport* keys)
{
    hid_keyboard_report_t report;
    report.reserved = 0;
    report.modifier = keys->modifiers;
    memcpy(report.keycode, keys->keys, 6);
    hid.SendReport(HID_REPORT_ID_KEYBOARD, &report, sizeof(report));
}

void USBKeyboardUtility::end()
{
    // do nothing
}