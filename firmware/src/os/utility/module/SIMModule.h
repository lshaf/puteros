//
// Created by L Shaf on 2026-01-16.
//

#pragma once

#include <Arduino.h>

class SIMModule
{
public:
    void begin(HardwareSerial *serial, int8_t txPin, int8_t rxPin);
    void end();

    String sendAndReceive(const String& command)
    {
        sendCommand(command);
        auto r = readResponse();

        String dc = String(command);
        dc.replace("\r", "\\r");
        dc.replace("\n", "\\n");
        Serial.println("Invoke command to SIM module: " + dc);

        String d = r;
        d.replace("\r", "\\r");
        d.replace("\n", "\\n");
        Serial.println("Response from SIM module: " + d);

        return r;
    }

    bool checkConnection();
    String getFirmwareVersion();
    String getCardStatus();
    String getIMEI();
    int getSignalQuality();

protected:
    void sendCommand(const String& command) const;
    String readResponse();

private:
    unsigned long timeoutMs = 10000;
    HardwareSerial* _serial = nullptr;
};
