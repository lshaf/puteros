//
// Created by L Shaf on 2026-01-21.
//

#pragma once
#include "os/core/ListScreen.hpp"

class GameMathScreen final : public ScreenState
{
public:
  void update() override;
  void render() override;
  void init() override;

  void renderMain();
  void renderGame();
  void renderResult();
  void playGame();

  void gameUpdater(Keyboard_Class* _keeb);
  void menuUpdater(Keyboard_Class* _keeb);
  void resultUpdater(Keyboard_Class* _keeb);

  bool isKeepScreenOn() override {return true;}

private:
  int8_t selectedMenu = 0;
  uint16_t score = 0;
  uint8_t gameMode = 1; // 1: Addition, 2: Subtraction, 3: Multiplication
  uint8_t totalDigit = 1; // 1: single-digit, 2: double-digit, 3: triple-digit
  String currentAnswer = "";
  unsigned long startTime = 0;
  int gameTime = 95; // in seconds
  int firstDigit = 0;
  int secondDigit = 0;
  unsigned long lastRender = 0;
  enum
  {
    STATE_MAIN,
    STATE_PLAYING,
    STATE_RESULT
  } gameState = STATE_MAIN;

  void generateQuestion()
  {
    firstDigit = getRandomDigit();
    secondDigit = getRandomDigit();
    if (gameMode == 2 && secondDigit > firstDigit)
    {
      std::swap(firstDigit, secondDigit);
    }
  }

  String getAnswer()
  {
    switch (gameMode)
    {
      case 1: return String(firstDigit + secondDigit);
      case 2: return String(firstDigit - secondDigit);
      case 3: return String(firstDigit * secondDigit);
      default: return "0";
    }
  }

  int getRandomDigit()
  {
    if (totalDigit == 1) return random(0, 10);
    if (totalDigit == 2) return random(10, 100);
    if (totalDigit == 3) return random(100, 1000);
    return 0;
  }

  String getModeSymbol()
  {
    switch (gameMode)
    {
      case 1: return "+";
      case 2: return "-";
      case 3: return "*";
      default: return "?";
    }
  }

  String getModeLabel()
  {
    switch (gameMode)
    {
      case 1: return "Addition";
      case 2: return "Subtraction";
      case 3: return "Multiplication";
      default: return "Unknown";
    }
  }

  String getDigitLabel()
  {
    switch (totalDigit)
    {
      case 1: return "Single";
      case 2: return "Double";
      case 3: return "Triple";
      default: return "Unknown";
    }
  }
};
