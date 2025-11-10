//
// Created by l-sha on 31/10/2025.
//

#include <SD.h>
#include "lgfx/utility/lgfx_qrcode.h"

#include "os/screens/utility/UtilityQRCodeScreen.h"
#include "os/component/InputFileScreen.h"
#include "os/component/InputTextScreen.hpp"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/utility/UtilityMenuScreen.h"

void UtilityQRCodeScreen::generateFolder()
{
  HelperUtility::makeDirectoryRecursive(qrPath);
}


void UtilityQRCodeScreen::init()
{
  currentState = STATE_MENU;
  Template::renderHead("QRCode");
  setEntries({
    {"Mode", currentMode == MODE_WRITE ? "Write" : "File"},
    {"Inverted", isInverted ? "Yes" : "No"},
    {"Generate"}
  });
}

void UtilityQRCodeScreen::refreshOption()
{
  entries = {
    {"Mode", currentMode == MODE_WRITE ? "Write" : "File"},
    {"Inverted", isInverted ? "Yes" : "No"},
    {"Generate"}
  };

  render();
}

void UtilityQRCodeScreen::renderPathEntries(const std::string& path)
{
  currentState = STATE_SELECT_FILE;

  Template::renderHead("QR File");
  auto dir = SD.open(path.c_str());
  File currentFile;
  std::vector<ListEntryItem> fileEntries;
  while ((currentFile = dir.openNextFile()))
  {
    const std::string name = currentFile.name();
    fileEntries.push_back({
      name,
      currentFile.isDirectory() ? "DIR" : "FIL"
    });
    currentFile.close();
  }
  dir.close();

  setEntries(fileEntries);
}


void UtilityQRCodeScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_QRCODE)
  {
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    init();
  } else if (currentState == STATE_MENU)
  {
    if (entry.label == "Inverted")
    {
      isInverted = !isInverted;
      refreshOption();
    } else if (entry.label == "Mode")
    {
      currentMode = (currentMode == MODE_WRITE) ? MODE_FILE : MODE_WRITE;
      refreshOption();
    } else if (entry.label == "Generate")
    {
      if (currentMode == MODE_FILE)
      {
        generateFolder();
        currentPath = qrPath;
        renderPathEntries(qrPath);
      } else
      {
        const std::string data = InputTextScreen::popup("QR Data");
        if (data.empty())
        {
          init();
          return;
        }

        currentState = STATE_QRCODE;
        if (!generateQRCode(data))
        {
          Template::renderStatus("QRCode generation is failed");
          HelperUtility::delayMs(1500);
          init();
        }
      }
    }
  } else if (currentState == STATE_SELECT_FILE)
  {
    if (entry.value == "DIR")
    {
      if (currentPath == "")
        currentPath += entry.label;
      else
        currentPath += "/" + entry.label;

      renderPathEntries(currentPath);
    } else
    {
      if (currentPath == "")
        currentPath = "/" + entry.label;
      else
        currentPath = currentPath + "/" + entry.label;

      File qrFile = SD.open(currentPath.c_str());
      if (!qrFile)
      {
        Template::renderStatus("Failed to open file");
        HelperUtility::delayMs(1500);
        renderPathEntries(qrPath);
        return;
      }

      if (qrFile.size() > 1800)
      {
        Template::renderStatus("File size is too large");
        HelperUtility::delayMs(1500);
        renderPathEntries(qrPath);
        qrFile.close();
        return;
      }

      currentState = STATE_QRCODE;
      if (!generateQRCode(qrFile))
      {
        Template::renderStatus("QRCode generation is failed");
        HelperUtility::delayMs(1500);
        init();
      }
    }
  }
}

void UtilityQRCodeScreen::onBack()
{
  if (currentState == STATE_MENU)
    _global->setScreen(new UtilityMenuScreen());
  else if (currentState == STATE_QRCODE)
  {
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);
    init();
  } else if (currentState == STATE_SELECT_FILE)
  {
    if (currentPath == qrPath)
    {
      init();
    } else
    {
      const int lastSlash = currentPath.rfind('/');
      if (lastSlash > 0)
        currentPath = currentPath.substr(0, lastSlash);
      else
        currentPath = "";

      renderPathEntries(currentPath);
    }
  }
}

void UtilityQRCodeScreen::onEscape()
{
  if (currentState == STATE_QRCODE)
    M5Cardputer.Lcd.fillScreen(TFT_BLACK);

  if (currentState == STATE_SELECT_FILE)
    init();
  else
    _global->setScreen(new MainMenuScreen());
}

bool UtilityQRCodeScreen::generateQRCode(File& qrFile)
{
  std::string data = "";

  const size_t fileSize = qrFile.size();
  data.resize(fileSize);

  const size_t readBytes = qrFile.readBytes(&data[0], fileSize);
  if (readBytes == 0) {
    return false;
  }

  if (readBytes != fileSize) data.resize(readBytes);
  qrFile.close();

  return generateQRCode(data);
}


bool UtilityQRCodeScreen::generateQRCode(const std::string& data)
{
  const auto _lcd = &M5Cardputer.Lcd;
  const int w = _lcd->height();
  const int x = _lcd->width() / 2 - w / 2;
  const int y = 0;
  const char* string = data.c_str();
  bool isPrinted = false;

  for (int version = 1; version <= 40; ++version)
  {
    QRCode qrcode;
    auto qrcodeData = (uint8_t*)alloca(lgfx_qrcode_getBufferSize(version));
    if (0 != lgfx_qrcode_initText(&qrcode, qrcodeData, version, 0, string)) continue;
    int_fast16_t thickness = w / qrcode.size;
    int_fast16_t lineLength = qrcode.size * thickness;
    int_fast16_t offset = (w - lineLength) >> 1;

    _lcd->startWrite();
    _lcd->fillScreen(isInverted ? TFT_BLACK : TFT_WHITE);
    _lcd->writeFillRect(x, y, w, offset, isInverted ? TFT_BLACK : TFT_WHITE);
    int_fast16_t dy = y + offset;
    if (thickness)
    {
      int_fast16_t iy = 0;
      do {
        _lcd->writeFillRect(x, dy, offset, thickness, isInverted ? TFT_BLACK : TFT_WHITE);
        int_fast16_t ix = 0;
        int_fast16_t dx = x + offset;
        do {
          if (isInverted)
            _lcd->setColor(lgfx_qrcode_getModule(&qrcode, ix, iy) ? TFT_WHITE : TFT_BLACK);
          else
            _lcd->setColor(lgfx_qrcode_getModule(&qrcode, ix, iy) ? TFT_BLACK : TFT_WHITE);

          _lcd->writeFillRect(dx, dy, thickness, thickness);
          dx += thickness;
        } while (++ix < qrcode.size);
        _lcd->writeFillRect(dx, dy, x + w - dx, thickness, isInverted ? TFT_BLACK : TFT_WHITE);
        dy += thickness;
      } while (++iy < qrcode.size);
    }
    _lcd->writeFillRect(x, dy, w, y + w - dy, isInverted ? TFT_BLACK : TFT_WHITE);
    _lcd->endWrite();
    isPrinted = true;
    break;
  }

  return isPrinted;
}