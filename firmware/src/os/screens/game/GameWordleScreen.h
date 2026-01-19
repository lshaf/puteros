//
// Created by l-sha on 07/12/2025.
//

#pragma once
#include "os/core/ScreenState.hpp"
#include "os/utility/CircularArray.h"


class GameWordleScreen final : public ScreenState
{
public:
  bool isKeepScreenOn() override {return true;}
  enum Language : uint8_t
  {
    ID = 0,
    EN = 1
  };

  enum State_e
  {
    STATE_MAIN_MENU,
    STATE_PLAY,
    STATE_RESULT
  } currentState = STATE_MAIN_MENU;

  void navigate(const State_e newState)
  {
    currentState = newState;
    render();
  }

  GameWordleScreen(const Language langCode) : currentLanguage(langCode) {};
  void init() override;
  void update() override;
  void render() override;

  void initGame();
  void resetGame();
  void riseDifficulty();
  void renderMainMenu();
  void renderGamePlay();
  void renderResult(bool isWin);

private:
  unsigned long startTime;
  enum Difficulty
  {
    DIF_EASY,
    DIF_MEDIUM,
    DIF_HARD,
  } currentDifficulty = DIF_EASY;

  int8_t currentCursor = 0;
  std::array<char, 5> currentInput = {};
  std::array<char, 5> chosenWord = {};

  bool useCommon = true;
  uint8_t totalAttempt = 0;
  std::array<char, 26> alphabetUsed = {};
  CircularBuffer<std::array<char, 5>> playerInput{7};

  int8_t selectedMenu = 0;
  Language currentLanguage;

  uint8_t getMaxAttempt() const
  {
    switch (currentDifficulty)
    {
    case DIF_EASY: return 10;
    case DIF_MEDIUM: return 7;
    case DIF_HARD: return 7;
    default: return 5;
    }
  }

  bool turnOnHelp() const
  {
    return currentDifficulty != DIF_HARD;
  }

  std::string getDifficultyStr() const
  {
    switch (currentDifficulty)
    {
    case DIF_EASY: return "Easy";
    case DIF_MEDIUM: return "Medium";
    case DIF_HARD: return "Hard";
    default: return "Unknown";
    }
  }

  int getColorGuess(const uint8_t index, const char guessedChar) const
  {
    if (guessedChar == '\0') return TFT_DARKGREY;
    if (guessedChar == chosenWord[index]) return TFT_DARKGREEN;
    for (int l = 0;l < 5;l++)
    {
      if (guessedChar == chosenWord[l]) return TFT_ORANGE;
    }
    return TFT_RED;
  }
};
