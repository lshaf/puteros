//
// Created by L Shaf on 2026-01-16.
//

#include "SIMModule.h"

void SIMModule::begin(HardwareSerial *serial, const int8_t txPin, const int8_t rxPin)
{
    _serial = serial;
    serial->begin(115200, SERIAL_8N1, rxPin, txPin);
}

void SIMModule::sendCommand(const String& command) const
{
    _serial->print(command);
}

String SIMModule::readResponse()
{
    String response;
    unsigned long startTime = millis();
    while (millis() - startTime < timeoutMs)
    {
        while (_serial->available())
        {
            const String s = _serial->readString();
            response += s;
            startTime = millis();
        }

        if (!response.isEmpty()) break;
        delay(10);
    }

    return response;
}

bool SIMModule::checkConnection()
{
    const String response = sendAndReceive("AT\r\n");
    return response.indexOf("OK") != -1;
}

String SIMModule::getFirmwareVersion()
{
    return sendAndReceive("AT+CGMR\r\n");
}

String SIMModule::getCardStatus()
{
    return sendAndReceive("AT+CPIN?\r\n");
}

String SIMModule::getIMEI()
{
    // r = AT+CGSN\r\n863866071636971\r\nOK\r\m
    String r = sendAndReceive("AT+CGSN\r\n");
    const int index = r.indexOf("\r\n");
    if (index != -1)
    {
        r = r.substring(index + 2);
        const int okIndex = r.indexOf("\r\nOK");
        if (okIndex != -1)
        {
            r = r.substring(0, okIndex);
        }
        r.trim();
    }
    return r;
}

int SIMModule::getSignalQuality()
{
    const String response = sendAndReceive("AT+CSQ\r\n");
    const int index = response.indexOf("+CSQ: ");
    if (index != -1)
    {
        const int commaIndex = response.indexOf(',', index);
        if (commaIndex != -1)
        {
            const String rssiStr = response.substring(index + 6, commaIndex);
            const int rssi = rssiStr.toInt();
            if (rssi == 99) return -1; // Unknown or undetectable
            return rssi;
        }
    }
    return -1; // Error
}

void SIMModule::end()
{
    _serial->flush();
    while (_serial->available()) _serial->read();
    _serial->end();
}