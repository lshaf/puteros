//
// Created by l-sha on 9/22/2025.
//

#pragma once

#include <M5Cardputer.h>
#include "BatteryIndicator.hpp"

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
    M5Cardputer.Lcd.drawRoundRect(3, 22, M5Cardputer.Lcd.width() - 6, M5Cardputer.Lcd.height() - 25, 4, BLUE);

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

  static void drawStatusBody(const std::string& status)
  {
    auto body = createBody();
    body.setTextColor(TFT_WHITE);
    body.drawCenterString(status.c_str(), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
    renderBody(&body);
  }
};