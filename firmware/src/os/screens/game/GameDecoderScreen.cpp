#include "os/screens/game/GameDecoderScreen.h"
#include "os/component/Template.hpp"
#include "os/screens/game/GameMenuScreen.h"
#include "os/utility/AudioUtility.h"

void GameDecoderScreen::init()
{
  Template::renderHead("HEX Decoder");
  render();
}

void GameDecoderScreen::update()
{
  if (currentState == STATE_PLAY && millis() - lastRender >= 500)
  {
    if (millis() >= endTime)
    {
      renderResult(false);
    } else
    {
      render();
    }
  }

  const auto _keyboard = &M5Cardputer.Keyboard;
  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    if (currentState == STATE_MAIN_MENU)
    {
      if (_keyboard->isKeyPressed('.'))
      {
        currentMenu = currentMenu + 1;
        if (currentMenu > 2) currentMenu = 0;
        render();
      } else if (_keyboard->isKeyPressed(';'))
      {
        currentMenu = currentMenu - 1;
        if (currentMenu < 0) currentMenu = 2;
        render();
      } else if (_keyboard->isKeyPressed(KEY_ENTER))
      {
        if (currentMenu == 0)
        {
          initGame();
        }
        else if (currentMenu == 2) _global->setScreen(new GameMenuScreen());
        else {
          currentDifficulty = currentDifficulty + 1;
          if (currentDifficulty > 3) currentDifficulty = 0;

          render();
        }
      }
    } else if (currentState == STATE_PLAY)
    {
      const Keyboard_Class::KeysState _keyState = _keyboard->keysState();

      for (const auto c : _keyState.word)
      {
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f'))
        {
          if (currentInputCursor < 4)
          {
            char finalInput = c;
            if (c >= 'a' && c <= 'f')
              finalInput = static_cast<char>(c - ('a' - 'A'));
            currentInput[currentInputCursor] = finalInput;
            currentInputCursor++;
            render();
          }
        }
      }

      if (_keyboard->isKeyPressed(KEY_ENTER))
      {
        bool hasBlank = false;
        for (uint8_t i = 0; i < 4; i++)
        {
          if (currentInput[i] == '\0')
          {
            hasBlank = true;
            break;
          }
        }

        if (hasBlank) return;

        if (currentInput == targetNumber)
        {
          renderResult(true);
          return;
        }

        playerInput.push(currentInput);
        totalUserInput++;
        currentInput.fill('\0');
        currentInputCursor = 0;

        if (getMaxAttempt() == -1 || totalUserInput < getMaxAttempt())
        {
          render();
        } else {
          renderResult(false);
        }
      } else if (_keyboard->isKeyPressed(KEY_BACKSPACE))
      {
        currentInputCursor--;
        if (currentInputCursor < 0) currentInputCursor = 0;
        currentInput[currentInputCursor] = '\0';
        render();
      } else if (_keyboard->isKeyPressed('`'))
      {
        navigate(STATE_MAIN_MENU);
      }
    } else if (currentState == STATE_RESULT)
    {
      if (_keyboard->isKeyPressed(KEY_BACKSPACE)
        || _keyboard->isKeyPressed(KEY_ENTER)
        || _keyboard->isKeyPressed('`'))
      {
        navigate(STATE_MAIN_MENU);
      }
    }
  }
}

void GameDecoderScreen::render()
{
  lastRender = millis();
  if (currentState == STATE_MAIN_MENU)
  {
    renderMainMenu();
  } else if (currentState == STATE_PLAY)
  {
    renderGamePlay();
  }
}

void GameDecoderScreen::renderMainMenu()
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
    if (i == currentMenu) textColor = _global->getMainColor();

    body.setTextColor(textColor);
    body.drawCenterString(menuItems[i].c_str(), body.width() / 2, startY + i * (body.fontHeight() + marginMenu));
  }

  Template::renderBody(&body);
}

void GameDecoderScreen::initGame()
{
  startTime = millis();
  endTime = millis() + getTimer() * 1000;
  for (int i = 0; i < 4; i++)
  {
    targetNumber[i] = charDatabase[HelperUtility::trueRandom(16)];
  }

  navigate(STATE_PLAY);
}

void GameDecoderScreen::renderGamePlay()
{
  auto body = Template::createBody();

  body.setTextSize(1.5);
  body.setTextColor(TFT_WHITE);
  for (uint8_t ci = 1; ci <= 4; ci++)
  {
    const char character = currentInput[ci - 1];
    auto color = (ci == (currentInputCursor + 1)) ? TFT_DARKCYAN : TFT_DARKGREY;
    body.fillRoundRect(17 * ci, 0, 15, 15, 2, color);
    if (character != '\0')
    {
      body.drawCentreString(std::string(1, character).c_str(), 17 * ci + 7, 2);
    }
  }

  const int colors[4] = {TFT_DARKGREY, TFT_RED, TFT_ORANGE, TFT_GREEN};
  for (int8_t t = 0;t < 6;t++)
  {
    int guessed[4] = {0, 0, 0, 0};
    const int startTop = (t + 1) * 17;
    const int topIndex = std::min(7, static_cast<int>(totalUserInput)) - (t + 1);
    for (uint8_t ci = 1; ci <= 4; ci++)
    {
      const auto character = playerInput.get(topIndex)[ci - 1];

      auto color = 0;
      if (topIndex >= 0) color = getColorGuess(ci - 1, character);
      guessed[ci - 1] = color;

      body.fillRoundRect(17 * ci, startTop, 15, 15, 2, currentDifficulty > 1 ? TFT_DARKGREY : colors[color]);
      if (topIndex >= 0 && character != '\0')
      {
        body.drawCentreString(std::string(1, character).c_str(), 17 * ci + 7, startTop + 2);
      }
    }

    uint8_t counter = 0;
    std::sort(std::begin(guessed), std::end(guessed));
    for (uint8_t y = 0; y < 2; y++)
    {
      for (uint8_t x = 0; x < 2; x++)
      {
        auto color = colors[guessed[counter]];
        body.fillRoundRect(x * 7 + 1, startTop + y * 7 + 1, 6, 6, 1, color);
        counter++;
      }
    }
  }

  unsigned long remainingTime = 0;
  if (millis() < endTime)
    remainingTime = static_cast<int>((endTime - millis()) / 1000);

  body.setTextSize(2);
  if (getMaxAttempt() == -1)
  {
    body.drawCenterString("Turn", 156, 2);
    body.drawCenterString(std::to_string(totalUserInput).c_str(), 156, 24);
  } else
  {
    body.drawCenterString("Attempt", 156, 2);
    body.drawCenterString(std::to_string(getMaxAttempt() - totalUserInput).c_str(), 156, 24);
  }
  body.drawCenterString("Timer", 156, body.height() - 38);
  body.drawCenterString(std::to_string(remainingTime).c_str(), 156, body.height() - 16);

  Template::renderBody(&body);
}

void GameDecoderScreen::renderResult(const bool isWin)
{
  currentState = STATE_RESULT;
  auto body = Template::createBody();
  body.setTextSize(2);
  if (isWin)
  {
    AudioUtility::playWin();
    body.setTextColor(TFT_GREEN);
    body.drawCenterString("You Win!", body.width() / 2, body.height() / 2 - body.fontHeight());
  } else
  {
    AudioUtility::playLose();
    body.setTextColor(TFT_RED);
    body.drawCenterString("Game Over!", body.width() / 2, body.height() / 2 - body.fontHeight());
  }

  body.setTextColor(TFT_WHITE);
  body.setTextSize(1);
  body.drawCenterString(("Answer: " + std::string(targetNumber.data(), 5)).c_str(), body.width() / 2, body.height() / 2 + 2);
  body.drawCenterString(("Turn: " + std::to_string(totalUserInput)).c_str(), body.width() / 2, body.height() / 2 + body.fontHeight() + 4);

  const unsigned long elapsedTime = (millis() - startTime) / 1000;
  const auto elapsedMinute = static_cast<uint16_t>(elapsedTime / 60);
  const auto elapsedSecond = static_cast<uint8_t>(elapsedTime % 60);
  const std::string timeStr = "Time: " + std::to_string(elapsedMinute) + "m " + std::to_string(elapsedSecond) + "s";
  body.drawCenterString(timeStr.c_str(), body.width() / 2, body.height() / 2 + (body.fontHeight() * 2) + 6);
  Template::renderBody(&body);
}

void GameDecoderScreen::resetState()
{
  playerInput.clear();
  startTime = 0;
  currentInputCursor = 0;
  currentInput = {0, 0, 0, 0};
  totalUserInput = 0;
}