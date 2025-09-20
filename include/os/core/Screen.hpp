//
// Created by L Shaf on 18/09/25.
//

#pragma once

#include <M5Cardputer.h>

#include "os/component/BatteryIndicator.hpp"

class Screen
{
private:
  bool batteryShouldRefresh() const
  {
    return (millis() - lastBatteryTimeRender) > 30000;
  }
protected:
  M5Canvas _header;
  M5Canvas _body;
  Keyboard_Class* _keyboard;

  unsigned long lastBatteryTimeRender = 0;
  bool hideBattery = false;

  typedef enum
  {
    RENDER_ALL,
    RENDER_HEADER,
    RENDER_BODY
  } RenderType;
public:
  Screen()
    : _header(&M5Cardputer.Lcd), _body(&M5Cardputer.Lcd), _keyboard(&M5Cardputer.Keyboard)
  {
    _header.createSprite(M5Cardputer.Lcd.width() - 6, 16);
    _body.createSprite(M5Cardputer.Lcd.width() - 16, M5Cardputer.Lcd.height() - 34);
  }

  virtual ~Screen() = default;
  virtual void update() = 0;

  void setTitle(const std::string& title)
  {
    _header.fillSprite(TFT_BLACK);
    _header.setTextColor(TFT_WHITE);
    _header.setTextSize(1.5);
    _header.drawString(title.c_str(), 2, _header.height() / 2 - _header.fontHeight() / 2);
  }

  void refreshBattery()
  {
    if (batteryShouldRefresh())
    {
      render(RENDER_HEADER);
    }
  }

  void render(const RenderType type = RENDER_ALL)
  {
    M5Cardputer.Lcd.drawRoundRect(3, 22, M5Cardputer.Lcd.width() - 6, M5Cardputer.Lcd.height() - 25, 4, BLUE);
    if (!hideBattery)
    {
      lastBatteryTimeRender = millis();
      drawBatteryIndicator(&_header, _header.width() - 29, 2, M5Cardputer.Power.getBatteryLevel());
    }
    if (type == RENDER_HEADER || type == RENDER_ALL) _header.pushSprite(3, 3);
    if (type == RENDER_BODY || type == RENDER_ALL) _body.pushSprite(8, 26);
  }
};
