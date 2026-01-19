//
// Created by L Shaf on 2026-01-16.
//

#include "ModuleCATMScreen.h"

#include "ModuleMenuScreen.h"

void ModuleCATMScreen::init()
{
  Template::renderHead("Unit Cat-M");
  Template::renderStatus("Checking module...");
  _gsm.begin(&Serial2, 2, 1);
  if (!_gsm.checkConnection())
  {
    Template::renderStatus("CAT-M Module not found", TFT_RED);
    HelperUtility::delayMs(2000);
    _global->setScreen(new ModuleMenuScreen());
    return;
  }

  const String imei = _gsm.getIMEI();
  const String imsi = _gsm.getCardStatus();
  const int signalQuality = _gsm.getSignalQuality();

  setEntries({
    {"IMEI", imei.c_str()},
    {"IMSI", imsi.c_str()},
    {"Signal Quality", String(signalQuality).c_str()}
  });
}

void ModuleCATMScreen::onEnter(ListEntryItem entry)
{
  // No action on enter
}

void ModuleCATMScreen::onBack()
{
  _global->setScreen(new ModuleMenuScreen());
}
