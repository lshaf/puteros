# PuterOS

PuterOS is a compact, modular firmware for the M5Stack Cardputer (ESP32-S3). It provides a screen-driven UI and a set of tools that demonstrate and leverage the hardware capabilities of the device.

## Features
- Modular screen-based UI with many built-in utilities and attack/demo tools
- WiFi tools: analyzers, packet monitor, deauther, ESP-NOW chat, network utilities
- BLE tools: BLE analyzer and HID (keyboard) utilities
- NFC/RFID module support (PN532, MFRC522)
- File manager (view files on SD)
- Utilities: QR code generator, I2C detector, audio playback helpers
- Game/demo screens
- Configurable settings (brightness, device info)

## High-level overview

- **Purpose**: provide a small, extensible platform that exposes WiFi/BLE/NFC, hardware modules, and utilities through a simple menu-and-screen interface.
- **Target**: M5Stack Cardputer (ESP32-S3) running under PlatformIO / Arduino environment.
- **Design**: modular screens (one feature = one screen), lightweight utilities, and optional hardware module support.

## Features

- **WiFi**
  - Scanning and analyzing nearby networks.
  - Packet capture/monitoring and small demo attacks (deauth, beacon spam).
  - Network utilities such as IP scanning and status info.
  - ESP-NOW chat demo for device-to-device messaging.

- **BLE**
  - BLE scanning and advertisement analysis.
  - BLE HID keyboard (duckyscript runner / shortcuts) to send keystrokes to a host.

- **Modules**
  - NFC MFRC522 (using I2C and and for now only able to read and check default keys)
  - PN5322 Killer (using uart and for now only able to read and check default keys)

- **File manager**
  - Browse files on SD card.

- **Utilities**
  - QR code generator and renderer on the screen.
  - I2C bus scanner to detect connected sensors/modules.

- **Games**
  - Decoding Number, small game to guess random generated number.

## Folder Structure
- `src/` — implementation files (screens are under `src/os/screens/`).
- `include/` — public headers and shared interfaces.
- `lib/` — external libraries and helpers.
- `platformio.ini` — build configuration.

## Getting Started (quick)
Here is how to manually compile and upload to your M5Stack Cardputer.

1. Install [PlatformIO](https://platformio.org/) (VSCode or CLI).
2. Clone this repository
3. Connect your M5Stack Cardputer
4. Build and upload:
   ```
   pio run -t upload
   ```
   
## Credits
Thanks to some of the following projects, their code and community helped me a lot to get this far:
- [Bruce Firmware](https://github.com/pr3y/Bruce)
- [Evil M5](https://github.com/7h30th3r0n3/Evil-M5Project)
- [ESP32 BLE Combo](https://github.com/blackketter/ESP32-BLE-Combo)

## License
This project is open source. See LICENSE for details.
