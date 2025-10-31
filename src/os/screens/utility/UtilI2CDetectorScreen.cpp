//
// Created by l-sha on 23/10/2025.
//

#include "os/screens/utility/UtilI2CDetectorScreen.h"
#include "os/component/Template.hpp"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/utility/UtilityMenuScreen.h"
#include "os/utility/HelperUtility.h"

UtilI2CDetectorScreen::~UtilI2CDetectorScreen()
{
  M5Cardputer.Ex_I2C.release();
}

int UtilI2CDetectorScreen::hextToDigit(const char c)
{
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'A' && c <= 'F') return 10 + (c - 'A');
  return -1; // invalid hex digit
}


void UtilI2CDetectorScreen::init()
{
  bool started = M5Cardputer.Ex_I2C.begin();
  if (!started)
  {
    Template::renderStatus("I2C Init Failed");
    HelperUtility::delayMs(1500);
    _global->setScreen(new UtilityMenuScreen());
    return;
  }
  render();
}

void UtilI2CDetectorScreen::update()
{
  const auto _keyboard = &M5Cardputer.Keyboard;
  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    if (_keyboard->isKeyPressed('r'))
    {
      render();
    } else if (_keyboard->isKeyPressed(KEY_BACKSPACE))
    {
      _global->setScreen(new UtilityMenuScreen());
    } else if (_keyboard->isKeyPressed('`'))
    {
      _global->setScreen(new MainMenuScreen());
    }
  }
}

void UtilI2CDetectorScreen::render()
{
  Template::renderHead("I2C Detector");

  auto body = Template::createBody();

  // Table layout
  const int cols = 16;
  const int rows = 8;
  const int rowH = 10;
  const int colW = 10;

  // generate header
  for (int col = 0; col < cols; ++col)
  {
    body.setCursor((col + 1) * colW + 3, 1);
    body.printf("%X", col);
    body.drawLine((col + 1) * colW, 0, (col + 1) * colW, rowH * (rows + 1), WHITE);
  }
  for (int row = 0; row < rows; ++row)
  {
    body.setCursor(2, (row + 1) * rowH + 2);
    body.printf("%X", row);
    body.drawLine(0, (row + 1) * rowH, colW * (cols + 1), (row + 1) * rowH, WHITE);
  }

  for (uint8_t addr = 8; addr < 0x78; addr++)
  {
    if (!M5Cardputer.Ex_I2C.scanID(addr)) continue;

    char line[32];
    snprintf(line, sizeof(line), "%02X", addr);

    int y = (hextToDigit(line[0]) + 1) * rowH + 1;
    int x = (hextToDigit(line[1]) + 1) * colW + 1;

    body.fillRect(x, y, colW - 1, rowH - 1, GREEN);
  }

  body.drawString("esc: exit, back: menu, r: reload", 0, body.height() - body.fontHeight());
  Template::renderBody(&body);
}