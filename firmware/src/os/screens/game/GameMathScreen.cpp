//
// Created by L Shaf on 2026-01-21.
//

#include "GameMathScreen.h"

#include "GameMenuScreen.h"
#include "os/utility/AudioUtility.h"

void GameMathScreen::init()
{
  Template::renderHead("Math Game");
  render();
}

void GameMathScreen::render()
{
  if (gameState == STATE_PLAYING)
  {
    renderGame();
  } else if (gameState == STATE_RESULT)
  {
    renderResult();
  } else if (gameState == STATE_MAIN)
  {
    renderMain();
  }
}

void GameMathScreen::update()
{
  if (gameState == STATE_PLAYING)
  {
    if (millis() - lastRender >= 500) render();
    if (millis() - startTime >= gameTime * 1000)
    {
      gameState = STATE_RESULT;
      render();
      return;
    }
  }

  const auto _keeb = &M5Cardputer.Keyboard;
  if (_keeb->isChange() && _keeb->isPressed())
  {
    if (gameState == STATE_PLAYING) gameUpdater(_keeb);
    else if (gameState == STATE_RESULT) resultUpdater(_keeb);
    else if (gameState == STATE_MAIN) menuUpdater(_keeb);
  }
}

void GameMathScreen::menuUpdater(Keyboard_Class* _keeb)
{
  if (_keeb->isKeyPressed('.'))
  {
    selectedMenu++;
    if (selectedMenu > 3) selectedMenu = 0;
    render();
  } else if (_keeb->isKeyPressed(';'))
  {
    selectedMenu--;
    if (selectedMenu < 0) selectedMenu = 3;
    render();
  } else if (_keeb->isKeyPressed(KEY_ENTER))
  {
    if (selectedMenu == 0)
    {
      playGame();
    } else if (selectedMenu == 1)
    {
      gameMode++;
      if (gameMode > 3) gameMode = 1;
      if (gameMode == 3 && totalDigit == 3) totalDigit = 1; // limit multiplication to 2 digits
      render();
    } else if (selectedMenu == 2)
    {
      totalDigit++;
      if (totalDigit > 3) totalDigit = 1;
      if (gameMode == 3 && totalDigit == 3) totalDigit = 1; // limit multiplication to 2 digits
      render();
    } else if (selectedMenu == 3)
    {
      _global->setScreen(new GameMenuScreen());
    }
  }
}

void GameMathScreen::gameUpdater(Keyboard_Class* _keeb)
{
  const Keyboard_Class::KeysState _keyState = _keeb->keysState();

  for (const auto c : _keyState.word)
  {
    if (c >= '0' && c <= '9')
    {
      if (currentAnswer.length() < 4)
      {
        currentAnswer += String(c);
        render();
      }
    }
  }

  if (_keeb->isKeyPressed('`'))
  {
    gameState = STATE_MAIN;
    render();
  } else if (_keeb->isKeyPressed(KEY_BACKSPACE))
  {
    if (currentAnswer.length() > 0)
    {
      currentAnswer.remove(currentAnswer.length() - 1);
      render();
    }
  } else if (_keeb->isKeyPressed(KEY_ENTER))
  {
    if (currentAnswer == getAnswer())
    {
      AudioUtility::playCorrectAnswer();
      score++;
      generateQuestion();
      currentAnswer = "";
      render();
    } else
    {
      AudioUtility::playWrongAnswer();
      currentAnswer = "";
      render();
    }
  }
}

void GameMathScreen::resultUpdater(Keyboard_Class* _keeb)
{
  if (_keeb->isKeyPressed(KEY_BACKSPACE)
    || _keeb->isKeyPressed(KEY_ENTER)
    || _keeb->isKeyPressed('`'))
  {
    gameState = STATE_MAIN;
    render();
  }
}

void GameMathScreen::playGame()
{
  score = 0;
  currentAnswer = "";
  startTime = millis();
  HelperUtility::shuffleSeed();
  generateQuestion();
  gameState = STATE_PLAYING;
  render();
}

void GameMathScreen::renderMain()
{
  auto body = Template::createBody();
  const std::vector<String> menuItems = {
    "Start Game",
    getModeLabel(),
    getDigitLabel() + " Digits",
    "Exit"
  };

  body.setTextSize(2);
  const int marginMenu = 6;
  const int startY = (body.height() - (menuItems.size() * body.fontHeight() + (menuItems.size() - 1) * marginMenu)) / 2;
  for (size_t i = 0; i < menuItems.size(); i++)
  {
    auto textColor = TFT_WHITE;
    if (i == selectedMenu) textColor = _global->getMainColor();

    body.setTextColor(textColor);
    body.drawCenterString(menuItems[i], body.width() / 2, startY + i * (body.fontHeight() + marginMenu));
  }

  Template::renderBody(&body);
}

void GameMathScreen::renderGame()
{
  lastRender = millis();
  auto body = Template::createBody();
  body.setTextSize(2);

  const int timeLeft = gameTime - ((millis() - startTime) / 1000);
  const String question = String(firstDigit) + " " + getModeSymbol() + " " + String(secondDigit) + " = ?";
  body.drawCenterString(question, body.width() / 2, 2);
  body.drawRightString("[S] " + HelperUtility::padNumber(score, 3), body.width(), body.height() - body.fontHeight());
  body.drawString("[T] " + HelperUtility::padNumber(timeLeft, 3), 0, body.height() - body.fontHeight());

  body.setTextSize(3);
  body.drawCenterString(currentAnswer, body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}

void GameMathScreen::renderResult()
{
  auto body = Template::createBody();
  body.setTextSize(3);
  body.drawCenterString(String(score), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  const int fh3 = body.fontHeight();
  body.setTextSize(2);
  body.drawCenterString("You Scored", body.width() / 2, body.height() / 2 - fh3 / 2 - body.fontHeight() - 6);
  body.setTextSize(1);
  const String levelLabel = getDigitLabel() + " " + getModeLabel();
  body.drawCenterString("In " + levelLabel, body.width() / 2, body.height() / 2 + fh3 / 2 + 6);
  Template::renderBody(&body);
}
