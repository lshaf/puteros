//
// Created by l-sha on 22/01/2026.
//

#include "ModuleInfraredScreen.h"
#include <IRremote.h>

#include "ModuleMenuScreen.h"

void ModuleInfraredScreen::init()
{
  Template::renderHead("Unit IR");
  Template::renderStatus("Initializing IR Module...");
  IrReceiver.begin(1, false);
  IrSender.begin(2, false);
  setEntries({
    {"Receive IR Signal"},
    {"Send IR Signal"}
  });
}

void ModuleInfraredScreen::onEnter(ListEntryItem entry)
{
  //
}

void ModuleInfraredScreen::onBack()
{
  IrReceiver.end();
  _global->setScreen(new ModuleMenuScreen());
}

void ModuleInfraredScreen::update()
{
  if (IrReceiver.decode())
  {
    IrReceiver.printIRSendUsage(&Serial);
    IrReceiver.resume();
  }

  ListScreen::update();
}

void ModuleInfraredScreen::preRender(M5Canvas& body)
{
}