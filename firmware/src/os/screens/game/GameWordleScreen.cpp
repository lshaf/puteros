//
// Created by l-sha on 07/12/2025.
//

#include "GameWordleScreen.h"

#include "GameMenuScreen.h"
#include "os/component/Template.hpp"

#include "os/utility/wordle/id.h"
#include "os/utility/wordle/en.h"

void GameWordleScreen::init()
{
  if (currentLanguage == Language::ID)
    Template::renderHead("Wordle Indonesia");
  else
    Template::renderHead("Wordle English");
}

void GameWordleScreen::render()
{
  if (currentState == STATE_MAIN_MENU)
  {
    renderMainMenu();
  } else if (currentState == STATE_PLAY)
  {
    renderGamePlay();
  }
}

void GameWordleScreen::update()
{
  const auto _keyboard = &M5Cardputer.Keyboard;
  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    if (currentState == STATE_MAIN_MENU)
    {
      if (_keyboard->isKeyPressed('.'))
      {
        selectedMenu = selectedMenu + 1;
        if (selectedMenu > 2) selectedMenu = 0;
        render();
      } else if (_keyboard->isKeyPressed(';'))
      {
        selectedMenu = selectedMenu - 1;
        if (selectedMenu < 0) selectedMenu = 2;
        render();
      } else if (_keyboard->isKeyPressed(KEY_ENTER))
      {
        if (selectedMenu == 0)
        {
          initGame();
        } else if (selectedMenu == 1)
        {
          riseDifficulty();
        } else if (selectedMenu == 2)
        {
          _global->setScreen(new GameMenuScreen());
        }
      }
    } else if (currentState == STATE_PLAY)
    {
      const Keyboard_Class::KeysState _keyState = _keyboard->keysState();

      for (const auto c : _keyState.word)
      {
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
        {
          if (currentCursor < 5)
          {
            if (c >= 'a' && c <= 'z')
              currentInput[currentCursor] = static_cast<char>(c - ('a' - 'A'));
            else
              currentInput[currentCursor] = c;
            currentCursor++;
            render();
          }
        }
      }

      if (_keyboard->isKeyPressed(KEY_ENTER))
      {
        if (currentInput == chosenWord)
        {
          renderResult(true);
          return;
        }

        bool hasBlank = false;
        for (uint8_t i = 0; i < 5; i++)
        {
          if (currentInput[i] == '\0')
          {
            hasBlank = true;
            break;
          }
        }

        if (hasBlank) return;

        playerInput.push(currentInput);
        for (uint8_t i = 0; i < 5; i++)
        {
          const char c = currentInput[i];
          bool alreadyUsed = false;
          for (const auto usedChar : alphabetUsed)
          {
            if (usedChar == c)
            {
              alreadyUsed = true;
              break;
            }
          }
          if (!alreadyUsed)
          {
            for (auto& usedChar : alphabetUsed)
            {
              if (usedChar == '\0')
              {
                usedChar = c;
                break;
              }
            }
          }
        }

        totalAttempt++;
        currentInput.fill('\0');
        currentCursor = 0;

        if (totalAttempt < getMaxAttempt())
        {
          render();
        } else {
          renderResult(false);
        }
      } else if (_keyboard->isKeyPressed(KEY_BACKSPACE))
      {
        currentCursor--;
        if (currentCursor < 0) currentCursor = 0;
        currentInput[currentCursor] = '\0';
        render();
      } else if (_keyboard->isKeyPressed('`'))
      {
        resetGame();
        navigate(STATE_MAIN_MENU);
      }
    } else if (currentState == STATE_RESULT)
    {
      if (_keyboard->isKeyPressed(KEY_BACKSPACE)
        || _keyboard->isKeyPressed(KEY_ENTER)
        || _keyboard->isKeyPressed('`'))
      {
        resetGame();
        navigate(STATE_MAIN_MENU);
      }
    }
  }
}

void GameWordleScreen::riseDifficulty()
{
  if (currentDifficulty == DIF_EASY)
  {
    currentDifficulty = DIF_MEDIUM;
  } else if (currentDifficulty == DIF_MEDIUM)
  {
    currentDifficulty = DIF_HARD;
  } else
  {
    currentDifficulty = DIF_EASY;
  }

  render();
}

void GameWordleScreen::renderMainMenu()
{
  auto body = Template::createBody();
  const std::vector<std::string> menuItems = {
    "Play",
    getDifficultyStr(),
    "Exit"
  };

  body.setTextSize(2);
  const int marginMenu = 6;
  const int startY = (body.height() - (menuItems.size() * body.fontHeight() + (menuItems.size() - 1) * marginMenu)) / 2;
  for (size_t i = 0; i < menuItems.size(); i++)
  {
    auto textColor = TFT_WHITE;
    if (i == selectedMenu) textColor = TFT_BLUE;

    body.setTextColor(textColor);
    body.drawCenterString(menuItems[i].c_str(), body.width() / 2, startY + i * (body.fontHeight() + marginMenu));
  }

  Template::renderBody(&body);
}

void GameWordleScreen::initGame()
{
  // Select a random word from the word list based on the current language
  const auto& wordList = (currentLanguage == Language::ID) ? WORDLE_DB_ID : WORDLE_DB_EN;
  const size_t wordCount = (currentLanguage == Language::ID) ? WORDLE_DB_ID_COUNT : WORDLE_DB_EN_COUNT;
  const size_t randomIndex = HelperUtility::true_random(static_cast<long>(wordCount));
  const std::string& selectedWord = wordList[randomIndex];

  // Copy the selected word into chosenWord array
  for (size_t i = 0; i < 5; ++i)
  {
    chosenWord[i] = selectedWord[i];
  }

  startTime = millis();
  currentInput.fill('\0');
  currentCursor = 0;

  navigate(STATE_PLAY);
}

void GameWordleScreen::resetGame()
{
  alphabetUsed.fill('\0');
  totalAttempt = 0;
  playerInput.clear();
  chosenWord.fill('\0');
  currentInput.fill('\0');
  currentCursor = 0;
}

void GameWordleScreen::renderGamePlay()
{
  auto body = Template::createBody();

  body.setTextSize(1.5);
  body.setTextColor(TFT_WHITE);
  for (uint8_t ci = 0; ci < 5; ci++)
  {
    auto color = (ci == currentCursor) ? TFT_DARKCYAN : TFT_DARKGREY;
    body.fillRoundRect(17 * ci, 0, 15, 15, 2, color);
    if (currentInput[ci] != '\0')
    {
      body.drawCentreString(std::string(1, currentInput[ci]).c_str(), 17 * ci + 7, 2);
    }
  }

  for (int8_t t = 0;t < 6;t++)
  {
    const int topIndex = std::min(7, static_cast<int>(totalAttempt)) - (t + 1);
    for (uint8_t ci = 0; ci < 5; ci++)
    {
      const auto character = playerInput.get(topIndex)[ci];

      auto color = TFT_DARKGREY;
      if (topIndex >= 0) color = getColorGuess(ci, character);

      body.fillRoundRect(17 * ci, (t + 1) * 17, 15, 15, 2, color);
      if (topIndex >= 0 && character != '\0')
      {
        body.drawCentreString(std::string(1, character).c_str(), 17 * ci + 7, (t + 1) * 17 + 2);
      }
    }
  }

  body.setTextSize(2);
  int y = 2;
  body.drawCenterString("Attempt", 156, y);
  y += body.fontHeight();
  body.drawCenterString("left", 156, y);
  y += body.fontHeight() + 7;
  body.setTextSize(3);
  body.drawCenterString(std::to_string(getMaxAttempt() - totalAttempt).c_str(), 156, y);

  int initial_x = 100;
  int x = initial_x;
  y += body.fontHeight() + 7;
  body.setTextSize(1);
  const int bodyOffset = body.fontWidth() * 2;
  for (char c = 'A'; c <= 'Z'; c++)
  {
    auto color = turnOnHelp() ? TFT_WHITE : TFT_DARKGREY;
    if (turnOnHelp())
    {
      for (const auto usedChar : alphabetUsed)
      {
        if (usedChar == c)
        {
          color = TFT_DARKGREY;
          break;
        }
      }
    }

    body.setTextColor(color);
    body.drawString(std::string(1, c).c_str(), x, y);
    x += bodyOffset;
    if (x + bodyOffset > body.width())
    {
      x = initial_x;
      y += body.fontHeight() + 2;
    }
  }

  Template::renderBody(&body);
}

void GameWordleScreen::renderResult(const bool isWin)
{
  currentState = STATE_RESULT;
  auto body = Template::createBody();
  body.setTextSize(2);
  if (isWin)
  {
    body.setTextColor(TFT_GREEN);
    body.drawCenterString("You Win!", body.width() / 2, body.height() / 2 - body.fontHeight());
  } else
  {

    body.setTextColor(TFT_RED);
    body.drawCenterString("Game Over!", body.width() / 2, body.height() / 2 - body.fontHeight());
  }

  body.setTextColor(TFT_WHITE);
  body.setTextSize(1);
  body.drawCenterString(("Answer: " + std::string(chosenWord.data(), 5)).c_str(), body.width() / 2, body.height() / 2 + 2);
  body.drawCenterString(("Turn: " + std::to_string(totalAttempt)).c_str(), body.width() / 2, body.height() / 2 + body.fontHeight() + 4);

  const unsigned long elapsedTime = (millis() - startTime) / 1000;
  const auto elapsedMinute = static_cast<uint16_t>(elapsedTime / 60);
  const auto elapsedSecond = static_cast<uint8_t>(elapsedTime % 60);
  const std::string timeStr = "Time: " + std::to_string(elapsedMinute) + "m " + std::to_string(elapsedSecond) + "s";
  body.drawCenterString(timeStr.c_str(), body.width() / 2, body.height() / 2 + (body.fontHeight() * 2) + 6);
  Template::renderBody(&body);
}