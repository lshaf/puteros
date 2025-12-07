//
// Created by l-sha on 25/10/2025.
//

#include "os/screens/game/GameMenuScreen.h"

#include "GameWordleScreen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/game/GameDecoderScreen.h"

void GameMenuScreen::init()
{
  Template::renderHead("Games");
  setEntries({
    {"Number Decoder"},
    {"Wordle Indonesia"},
    {"Wordle English"},
  });
}

void GameMenuScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "Number Decoder")
  {
    _global->setScreen(new GameDecoderScreen());
  } else if (entry.label == "Wordle Indonesia")
  {
    _global->setScreen(new GameWordleScreen(GameWordleScreen::Language::ID));
  } else if (entry.label == "Wordle English")
  {
    _global->setScreen(new GameWordleScreen(GameWordleScreen::Language::EN));
  }
}

void GameMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}