# PuterOS

A modular, class-based operating system for the M5Stack Cardputer (ESP32-S3), built with PlatformIO, Arduino, and M5Stack libraries.

## Features
- WiFi Features
  - World Clock
  - WiFi Information
  - IP + Port Scanner
  - WiFi Deauther
  - Packet Monitor
  - WiFi Analyzer

## Folder Structure
- `src/` — Main source code
- `include/` — Public headers and interfaces
- `lib/` — External and shared libraries
- `test/` — Test code
- `platformio.ini` — PlatformIO project configuration

## Getting Started
1. Install [PlatformIO](https://platformio.org/)
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

## License
This project is open source. See LICENSE for details.


