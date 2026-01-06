//
// Created by l-sha on 9/22/2025.
//

#pragma once

#include <M5Cardputer.h>
#include "BatteryIndicator.hpp"
#include "lgfx/utility/lgfx_qrcode.h"
#include "os/utility/HelperUtility.h"
#include "os/GlobalState.hpp"

class Template
{
public:
  struct Size_t {
    int width;
    int height;
  };

  static Size_t headerSize()
  {
    return {M5Cardputer.Lcd.width() - 6, 16};
  }

  static Size_t bodySize()
  {
    return {M5Cardputer.Lcd.width() - 16, M5Cardputer.Lcd.height() - 34};
  }

  static void renderHead(const std::string& title, const bool withBattery = true)
  {
    M5Canvas head(&M5Cardputer.Lcd);
    const auto size = headerSize();
    head.createSprite(size.width, size.height);

    head.fillSprite(BLACK);
    head.setTextColor(TFT_WHITE);
    head.setTextSize(1.5);
    head.drawString(title.c_str(), 3, head.height() / 2 - head.fontHeight() / 2);

    if (withBattery)
    {
      drawBatteryIndicator(&head, head.width() - 26, 1, M5Cardputer.Power.getBatteryLevel());
    }

    head.pushSprite(3, 3);
    head.deleteSprite();
  }

  static void renderBody(M5Canvas* bodyContent = nullptr)
  {
    auto _g = &GlobalState::getInstance();
    M5Cardputer.Lcd.drawRoundRect(3, 22, M5Cardputer.Lcd.width() - 6, M5Cardputer.Lcd.height() - 25, 4, _g->getMainColor());

    M5Canvas body(&M5Cardputer.Lcd);
    const auto size = bodySize();
    body.createSprite(size.width, size.height);
    if (bodyContent != nullptr) bodyContent->pushSprite(&body, 0, 0);
    body.pushSprite(8, 26);
    body.deleteSprite();
  }

  static M5Canvas createBody()
  {
    M5Canvas body(&M5Cardputer.Lcd);
    const auto size = bodySize();
    body.createSprite(size.width, size.height);
    body.fillSprite(BLACK);
    return body;
  }

  static void clearBody()
  {
    auto body = createBody();
    renderBody(&body);
  }

  static void renderStatus(const std::string& status, const int color = TFT_WHITE)
  {
    auto body = createBody();
    body.setTextColor(color);
    HelperUtility::drawWrappedCenterString(body, status);
    renderBody(&body);
  }

  static void renderProgress(const uint8_t progress, const std::string& status, const int color = TFT_WHITE)
  {
    auto _g = &GlobalState::getInstance();
    auto body = createBody();
    body.setTextColor(color);
    body.drawCenterString(status.c_str(), body.width() / 2, body.height() / 2 - body.fontHeight());
    body.drawRect(6, body.height() / 2 + 4, body.width() - 12, 10, _g->getMainColor());
    body.fillRect(6, body.height() / 2 + 4, static_cast<float>(body.width() - 12) * progress / 100.0f, 10, _g->getMainColor());
    renderBody(&body);
  }

  static bool renderQRCode(const std::string& data, const bool isInverted = false)
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
};
