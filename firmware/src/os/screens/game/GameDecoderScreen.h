//
// Created by l-sha on 26/10/2025.
//

#pragma once
#include <M5Cardputer.h>
#include "os/core/ScreenState.hpp"
#include "os/utility/CircularArray.h"

class GameDecoderScreen final : public ScreenState
{
  enum State_e
  {
    STATE_MAIN_MENU,
    STATE_PLAY,
    STATE_RESULT,
  };

public:
  bool isKeepScreenOn() override {return true;}

  void init() override;
  void update() override;
  void render() override;

  void renderMainMenu();
  void renderGamePlay();
  void renderResult(bool isWin);
  void resetState();
  void navigate(const State_e newState)
  {
    currentState = newState;
    if (newState == STATE_MAIN_MENU)
      resetState();

    render();
  }

  void initGame();

  std::string getDifficultyStr() const
  {
    switch (currentDifficulty)
    {
      case 0: return "Easy";
      case 1: return "Medium";
      case 2: return "Hard";
      case 3: return "Extreme";
      default: return "Unknown";
    }
  }

  int getTimer() const
  {
    switch (currentDifficulty)
    {
      case 0: return 180;
      case 1: return 90;
      case 2: return 180;
      case 3: return 90;
      default: return 180;
    }
  }

  int getMaxAttempt() const
  {
    switch (currentDifficulty)
    {
      case 0: return 14;
      case 1: return 7;
      case 2: return -1;
      case 3: return -1;
      default: return 14;
    }
  }

  int getColorGuess(const uint8_t index, const char guessedNumber) const
  {
    if (guessedNumber == targetNumber[index]) return 3;
    for (int l = 0;l < 4;l++)
      if (guessedNumber == targetNumber[l])
        return 2;

    return 1;
  }

private:
  char charDatabase[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  State_e currentState = STATE_MAIN_MENU;
  unsigned long startTime = 0;
  unsigned long lastRender = 0;
  unsigned long endTime = 0;

  CircularBuffer<std::array<char, 4>> playerInput{7};
  int currentInputCursor = 0;
  uint8_t totalUserInput = 0;
  std::array<char, 4> currentInput = {};
  std::array<char, 4> targetNumber = {};
  uint8_t currentDifficulty = 0;
  int8_t currentMenu = 0;
};
