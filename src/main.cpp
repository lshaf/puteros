#include <Arduino.h>
#include <M5Cardputer.h>
#include <SD.h>

#include "os/ScreenManager.hpp"
#include "os/screens/FileNavigatorScreen.hpp"

#define SD_SPI_SCK_PIN  40
#define SD_SPI_MISO_PIN 39
#define SD_SPI_MOSI_PIN 14
#define SD_SPI_CS_PIN   12

ScreenManager screenManager;

void setup() {
    M5Cardputer.begin();

    Serial.begin(115200);
    Serial.println("Kernel initialized.");

    // SD Card Initialization
    SPI.begin(SD_SPI_SCK_PIN, SD_SPI_MISO_PIN, SD_SPI_MOSI_PIN, SD_SPI_CS_PIN);

    if (!SD.begin(SD_SPI_CS_PIN, SPI, 25000000)) {
        // Print a message if the SD card initialization fails, or if the SD card does not exist.
        // 如果SD卡初始化失败或者SD卡不存在，则打印消息。
        Serial.println("Card failed, or not present");
        while (1);
    }
    screenManager.setScreen(new FileNavigatorScreen("/"));
}

void loop() {
    M5Cardputer.update();
    screenManager.update();
    screenManager.render();
}