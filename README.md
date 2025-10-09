# PuterOS

A modular, class-based operating system for the M5Stack Cardputer (ESP32-S3), built with PlatformIO, Arduino, and M5Stack libraries.

## Features
- WiFi Features
  - World Clock
  - WiFi Information
  - IP + Port Scanner
  - Packet Monitor
  - WiFi Analyzer
  - WiFi Deauther
- File Manager
  - View only for now
- Setting
  - Brightness

## Folder Structure
- `src/` — Main source code
- `include/` — Public headers and interfaces
- `lib/` — External and shared libraries
- `test/` — Test code
- `platformio.ini` — PlatformIO project configuration

## Getting Started
Here is how to manually compile and upload to your M5Stack Cardputer.

### CLI
1. Install [PlatformIO](https://platformio.org/)
2. Clone this repository
3. Connect your M5Stack Cardputer
4. Build and upload:
   ```
   pio run -t upload
   ```
   
### VSCode
1. Install [VSCode](https://code.visualstudio.com/)
2. Install the [PlatformIO extension](https://platformio.org/install/ide?install=vscode)
3. Clone this repository
4. Open the project folder in VSCode
5. Connect your M5Stack Cardputer
6. Use the PlatformIO toolbar to build and upload the project

### CLion
1. Install [CLion](https://www.jetbrains.com/clion/)
2. Install the [PlatformIO plugin](https://plugins.jetbrains.com/plugin/10052-platformio-ide)
3. Clone this repository
4. Open `platformio.ini` as project in CLion
5. Connect your M5Stack Cardputer
6. Use the PlatformIO toolbar to build and upload the project

## Credits
Thanks to some of the following projects, their code and community helped me a lot to get this far:
- [Bruce Firmware](https://github.com/pr3y/Bruce)
- [Evil M5](https://github.com/7h30th3r0n3/Evil-M5Project)

## License
This project is open source. See LICENSE for details.


