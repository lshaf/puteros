//
// Created by l-sha on 07/12/2025.
//

#pragma once
#include "os/core/ScreenState.hpp"


class GameWordleScreen final : public ScreenState
{
public:
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
  enum class Difficulty : uint8_t
  {
    EASY = 10,
    MEDIUM = 7,
    HARD = 5,
  };

  int8_t currentCursor = 0;
  std::array<char, 5> currentInput = {};
  std::array<char, 5> chosenWord = {};

  uint8_t totalAttempt = 0;
  std::array<char, 5> playerInput[10] = {};

  uint8_t selectedMenu = 0;
  Language currentLanguage;
  Difficulty currentDifficulty = Difficulty::EASY;

  uint8_t getMaxAttempt() const
  {
    return static_cast<uint8_t>(currentDifficulty);
  }

  std::string getDifficultyStr() const
  {
    switch (currentDifficulty)
    {
    case Difficulty::EASY: return "Easy";
    case Difficulty::MEDIUM: return "Medium";
    case Difficulty::HARD: return "Hard";
    default: return "Unknown";
    }
  }

  int getColorGuess(const uint8_t index, const char guessedChar) const
  {
    if (guessedChar == '\0') return TFT_DARKGREY;
    if (guessedChar == chosenWord[index]) return TFT_DARKGREEN;
    for (int l = 0;l < 4;l++)
    {
      if (guessedChar == chosenWord[l]) return TFT_ORANGE;
    }
    return TFT_RED;
  }
};
