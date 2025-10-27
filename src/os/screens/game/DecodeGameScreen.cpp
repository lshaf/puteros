#include "os/screens/game/DecodeGameScreen.h"
#include "os/component/Template.hpp"
#include "os/screens/game/GameMenuScreen.h"
#include "os/utility/HelperUtility.h"

DecodeGameScreen::~DecodeGameScreen()
{
  playerInput->fill({});
}

void DecodeGameScreen::init()
{
  Template::renderHead("Number Decoder");
}

void DecodeGameScreen::update()
{
  if (currentState == STATE_PLAY && millis() - lastRender >= 500)
  {
    if (millis() >= endTime)
    {
      navigate(STATE_GAME_OVER);
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
          if (currentDifficulty > 2) currentDifficulty = 0;

          render();
        }
      }
    } else if (currentState == STATE_PLAY)
    {
      const Keyboard_Class::KeysState _keyState = _keyboard->keysState();

      for (const auto c : _keyState.word)
      {
        if (c >= '0' && c <= '9')
        {
          if (currentInputCursor < 4)
          {
            currentInput[currentInputCursor] = c - '0';
            currentInputCursor++;
            render();
          }
        }
      }

      if (_keyboard->isKeyPressed(KEY_ENTER))
      {
        if (currentInput == targetNumber)
        {
          navigate(STATE_WIN);
          return;
        }

        if (totalUserInput < maxAttempt)
        {
          playerInput[totalUserInput] = currentInput;
          totalUserInput++;
          currentInput = {0, 0, 0, 0};
          currentInputCursor = 0;
          render();
        } else {
          navigate(STATE_GAME_OVER);
        }
      } else if (_keyboard->isKeyPressed(KEY_BACKSPACE))
      {
        currentInput[currentInputCursor] = 0;
        currentInputCursor--;
        if (currentInputCursor < 0) currentInputCursor = 0;
        render();
      } else if (_keyboard->isKeyPressed('`'))
      {
        navigate(STATE_MAIN_MENU);
      }
    } else if (currentState == STATE_GAME_OVER || currentState == STATE_WIN)
    {
      if (_keyboard->isKeyPressed(KEY_BACKSPACE) || _keyboard->isKeyPressed(KEY_ENTER))
      {
        navigate(STATE_MAIN_MENU);
      }
    }
  }
}

void DecodeGameScreen::render()
{
  lastRender = millis();
  if (currentState == STATE_MAIN_MENU)
  {
    renderMainMenu();
  } else if (currentState == STATE_PLAY)
  {
    renderGamePlay();
  } else if (currentState == STATE_GAME_OVER)
  {
    renderGameOver();
  } else if (currentState == STATE_WIN)
  {
    renderWin();
  }
}

void DecodeGameScreen::renderMainMenu()
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
    if (i == currentMenu) textColor = TFT_BLUE;

    body.setTextColor(textColor);
    body.drawCenterString(menuItems[i].c_str(), body.width() / 2, startY + i * (body.fontHeight() + marginMenu));
  }

  Template::renderBody(&body);
}

void DecodeGameScreen::initGame()
{
  resetState();
  endTime = millis() + getTimer() * 1000;
  for (int i = 0; i < 4; i++)
  {
    targetNumber[i] = static_cast<uint8_t>(random(10));
  }

  navigate(STATE_PLAY);
}

void DecodeGameScreen::renderGamePlay()
{
  auto body = Template::createBody();

  int startY = 26;
  const int centerX = body.width() / 2  - 38;

  body.setTextDatum(middle_left);
  body.setTextSize(2);
  body.setCursor(centerX, 8);
  body.setTextColor(TFT_WHITE);

  for (int i = 0; i < 4; i++)
  {
    if (i == currentInputCursor) body.setTextColor(TFT_BLUE);
    else body.setTextColor(TFT_WHITE);
    body.print((std::to_string(currentInput[i]) + " ").c_str());
  }

  body.setTextDatum(middle_right);
  body.setTextSize(2);
  body.setTextColor(TFT_WHITE);

  int timeLeftInSeconds = (endTime - millis()) / 1000;
  if (timeLeftInSeconds < 0) timeLeftInSeconds = 0;
  char buff[4];
  sprintf(buff, "%03u", timeLeftInSeconds);
  body.drawRightString(buff, body.width() - 10, body.height() - body.fontHeight());

  body.setTextSize(4);
  body.setTextColor(TFT_WHITE);
  body.drawCenterString(
    std::to_string(maxAttempt - totalUserInput + 1).c_str(),
    body.width() - body.fontWidth() - 5,
    body.height() / 2 - body.fontHeight() + 6
  );

  for (int y = totalUserInput - 1; y >= max(0, totalUserInput - 5); y--)
  {
    uint8_t guessed[3] = {0, 0, 0};
    body.setTextDatum(middle_left);
    body.setTextSize(2);
    body.setCursor(centerX, startY);
    for (int x = 0; x < 4; x++)
    {
      int color;
      const int guessedColor = getColorGuess(x, playerInput[y][x]);
      if (currentDifficulty == 2) color = TFT_WHITE;
      else color = guessedColor;

      body.setTextColor(color);
      body.print((std::to_string(playerInput[y][x]) + " ").c_str());
      if (guessedColor == TFT_GREEN)
      {
        guessed[0]++;
      } else if (guessedColor == TFT_ORANGE)
      {
        guessed[1]++;
      } else
      {
        guessed[2]++;
      }
    }

    body.setCursor(centerX - 40, startY - 1);
    body.setTextDatum(middle_right);
    body.setTextSize(1);
    body.setTextColor(TFT_GREEN);
    body.print((std::to_string(guessed[0]) + " ").c_str());
    body.setTextColor(TFT_ORANGE);
    body.print((std::to_string(guessed[1]) + " ").c_str());
    body.setTextColor(TFT_RED);
    body.print((std::to_string(guessed[2]) + " ").c_str());

    startY = startY + (body.fontHeight() * 2) + 2;
  }

  Template::renderBody(&body);
}

void DecodeGameScreen::renderGameOver()
{
  auto body = Template::createBody();
  body.setTextSize(2);
  body.setTextColor(TFT_RED);
  body.drawCenterString("You Lose!", body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}

void DecodeGameScreen::renderWin()
{
  auto body = Template::createBody();
  body.setTextSize(2);
  body.setTextColor(TFT_GREEN);
  body.drawCenterString("You Win!", body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}

void DecodeGameScreen::resetState()
{
  playerInput->fill({});
  currentInputCursor = 0;
  currentInput = {0, 0, 0, 0};
  totalUserInput = 0;
}