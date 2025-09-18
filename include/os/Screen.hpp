//
// Created by L Shaf on 18/09/25.
//

#pragma once

#include <M5Cardputer.h>

class Screen
{
protected:
  M5Canvas _header;
  M5Canvas _body;
public:
  Screen()
    : _header(&M5Cardputer.Lcd), _body(&M5Cardputer.Lcd)
  {
    _header.createSprite(M5Cardputer.Lcd.width() - 6, 16);
    _body.createSprite(M5Cardputer.Lcd.width() - 16, M5Cardputer.Lcd.height() - 34);
  }

  virtual ~Screen() = default;
  virtual void update() = 0;
  void render()
  {
    M5Cardputer.Lcd.drawRoundRect(3, 22, M5Cardputer.Lcd.width() - 6, M5Cardputer.Lcd.height() - 25, 4, BLUE);
    _header.pushSprite(3, 3);
    _body.pushSprite(8, 26);
  }
};