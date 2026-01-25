//
// Created by l-sha on 22/01/2026.
//

#define SEND_PWM_BY_TIMER

#include "ModuleInfraredScreen.h"
#include "ModuleMenuScreen.h"
#include <IRremote.hpp>

void ModuleInfraredScreen::init()
{
  Template::renderHead("Unit IR");
  Template::renderStatus("Initializing IR Module...");
  IrReceiver.begin(1, false);
  IrSender.begin(2, false);
  setEntries({
    {"Add New Remote"},
    {"Emulate Remote"},
    {"TV B-Gone"}
  });
}

void ModuleInfraredScreen::onEnter(const ListEntryItem& entry)
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