//
// Created by L Shaf on 2026-01-16.
//

#pragma once

#include <Arduino.h>

class SIMModule
{
public:
    void begin(HardwareSerial *serial, int txPin, int rxPin);
    void end();

    String sendAndReceive(const String& command)
    {
        sendCommand(command);
        return readResponse();
    }

    bool checkConnection();
    String getFirmwareVersion();
    String getIMSI();
    String getIMEI();
    int getSignalQuality();

protected:
    void sendCommand(const String& command) const;
    String readResponse();

private:
    unsigned long timeoutMs = 10000;
    HardwareSerial* _serial = nullptr;
};
