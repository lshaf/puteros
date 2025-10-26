//
// Created by l-sha on 25/10/2025.
//

#include "os/screens/game/GameMenuScreen.h"

#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/game/DecodeGameScreen.h"

void GameMenuScreen::init()
{
  Template::renderHead("Games");
  setEntries({
    {"Number Decoder"},
  });
}

void GameMenuScreen::onEnter(ListEntryItem entry)
{
  if (entry.label == "Number Decoder")
  {
    _global->setScreen(new DecodeGameScreen());
  }
}

void GameMenuScreen::onBack()
{
  _global->setScreen(new MainMenuScreen());
}