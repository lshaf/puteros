//
// Created by l-sha on 26/10/2025.
//

#pragma once
#include <M5Cardputer.h>
#include "os/core/ScreenState.hpp"

class GameDecoderScreen final : public ScreenState
{
  static constexpr size_t maxAttempt = 6;
  enum State_e
  {
    STATE_MAIN_MENU,
    STATE_PLAY,
    STATE_GAME_OVER,
    STATE_WIN
  };

public:
  ~GameDecoderScreen() override;
  void init() override;
  void update() override;
  void render() override;

  void renderMainMenu();
  void renderGamePlay();
  void renderGameOver();
  void renderWin();
  void resetState();
  void navigate(const State_e newState)
  {
    currentState = newState;
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
      default: return "Unknown";
    }
  }

  int getTimer() const
  {
    switch (currentDifficulty)
    {
      case 0: return 120;
      case 1: return 75;
      case 2: return 90;
      default: return 120;
    }
  }

  int getColorGuess(const uint8_t index, const uint8_t guessedNumber) const
  {
    if (guessedNumber == targetNumber[index]) return TFT_GREEN;
    for (int l = 0;l < 4;l++)
    {
      if (guessedNumber == targetNumber[l]) return TFT_ORANGE;
    }
    return TFT_RED;
  }

private:
  State_e currentState = STATE_MAIN_MENU;
  unsigned long lastRender = 0;
  unsigned long endTime = 0;

  std::array<uint8_t, 4> playerInput[maxAttempt] = {};
  int currentInputCursor = 0;
  uint8_t totalUserInput = 0;
  std::array<uint8_t, 4> currentInput = {};
  std::array<uint8_t, 4> targetNumber = {};
  uint8_t currentDifficulty = 0;
  uint8_t currentMenu = 0;
};
