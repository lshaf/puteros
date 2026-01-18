//
// Created by L Shaf on 2026-01-17.
//

#include "ModuleCapLoraScreen.h"
#include "ModuleMenuScreen.h"

bool ModuleCapLoraScreen::isLoraAttached()
{
  _global->loadLora();
  const int state = lora.begin(
    LORA_FREQ, LORA_BW, LORA_SF, LORA_CR,
    LORA_SYNC_WORD, LORA_TX_POWER, LORA_PREAMBLE_LEN
  );
  if (state != RADIOLIB_ERR_NONE)
  {
    return false;
  }

  lora.sleep();
  lora.standby();
  return true;
}

void ModuleCapLoraScreen::init()
{
  Template::renderHead("Cap Lora-1262");
  gps_module.begin(&Serial0, 115200, 15);
  if (!isLoraAttached())
  {
    Template::renderStatus("Lora Module not found", TFT_RED);
    HelperUtility::delayMs(2000);
    onBack();
    return;
  }

  showMainMenuScreen();
}

void ModuleCapLoraScreen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MENU)
  {
    if (entry.label == "Chatting")
    {
      initChat();
    } else if (entry.label == "Lora Setting")
    {} else if (entry.label == "GPS Data")
    {
      currentState = STATE_GPS_DATA;
      render();
    } else if (entry.label == "Wardriving")
    {
      _global->loadSDCard();
      if (!_global->getIsSDCardLoaded())
      {
        Template::renderStatus("SD Card not loaded", TFT_RED);
        HelperUtility::delayMs(2000);
        showMainMenuScreen();
        return;
      }

      if (!gps_module.gps.location.isValid())
      {
        Template::renderStatus("GPS module is not ready. try go outside and for some time", TFT_ORANGE);
        HelperUtility::delayMs(2000);
        showMainMenuScreen();
        return;
      }

      if (gps_module.initWardrive() == false)
      {
        Template::renderStatus(("Wardrive Error: " + gps_module.getLastWardriveError()).c_str(), TFT_RED);
        HelperUtility::delayMs(2000);
        showMainMenuScreen();
        return;
      }

      currentState = STATE_WARDRIVING;
      render();
    }
  }
}

void ModuleCapLoraScreen::onBack()
{
  if (currentState == STATE_MENU)
  {
    gps_module.end();
    _global->loadSDCard();
    _global->setScreen(new ModuleMenuScreen());
  } else
  {
    if (currentState == STATE_WARDRIVING)
    {
      _global->loadLora();
      gps_module.endWardrive();
    }
    currentState = STATE_MENU;
    showMainMenuScreen();
  }
}

void ModuleCapLoraScreen::update()
{
  gps_module.update();
  if ((currentState == STATE_GPS_DATA || currentState == STATE_WARDRIVING) && millis() - lastRefreshTime > 500)
  {
    if (currentState == STATE_WARDRIVING) gps_module.doWardrive();
    render();
  }
  ListScreen::update();
}

void ModuleCapLoraScreen::render()
{
  if (currentState == STATE_MENU)
  {
    ListScreen::render();
  } else if (currentState == STATE_GPS_DATA)
  {
    showGSPDataScreen();
  } else if (currentState == STATE_WARDRIVING)
  {
    showWardriveScreen();
  }
}

void ModuleCapLoraScreen::showGSPDataScreen()
{
  Template::renderHead("Cap Lora-1262 GPS");
  lastRefreshTime = millis();
  auto gps = gps_module.gps;
  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.println(("LAT: " + String(gps.location.lat(), 9)).c_str());
  body.println(("LNG: " + String(gps.location.lng(), 9)).c_str());
  body.println(("SPEED: " + String(gps.speed.kmph(), 2) + " km/h").c_str());
  body.println(("COURSE: " + String(gps.course.deg(), 2) + " deg").c_str());
  body.println(("ALTITUDE: " + String(gps.altitude.meters(), 2) + " m").c_str());
  body.println(("SATELLITES: " + String(gps.satellites.value())).c_str());
  body.println(("DATE: " + gps_module.getCurrentGPSDate()).c_str());
  body.println(("TIME: " + gps_module.getCurrentGPSTime() + " UTC").c_str());
  Template::renderBody(&body);
}

void ModuleCapLoraScreen::showWardriveScreen()
{
  Template::renderHead("Wardriving");

  // get csv name from full path
  String csvName = gps_module.getCurrentFilename().c_str();

  auto gps = gps_module.gps;
  String status = "Idle";
  if (gps_module.statusWardrive() == GPSModule::WARDRIVE_STATE_SCANNING)
  {
    status = "Scanning...";
  } else if (gps_module.statusWardrive() == GPSModule::WARDRIVE_STATE_SAVING)
  {
    status = "Saving...";
  }
  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.drawString(gps_module.getCurrentGPSDate(), 0, 0);
  body.drawString(gps_module.getCurrentGPSTime() + " UTC", 0, body.fontHeight() + 2);
  body.drawString("File: " + csvName, 0, body.fontHeight() * 2 + 4);
  body.drawRightString(String(gps.location.lat(), 6), body.width(), 0);
  body.drawRightString(String(gps.location.lng(), 6), body.width(), body.fontHeight() + 2);
  body.drawString("Esc to exit", 0, body.height() - body.fontHeight());
  body.drawRightString(status, body.width(), body.height() - body.fontHeight());
  body.drawCenterString("WiFi Discovered", body.width() / 2, body.height() / 2 + body.fontHeight() + 2);
  body.setTextSize(2);
  body.drawCenterString(String(gps_module.getDiscoveredWifi()), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
  Template::renderBody(&body);
}

void ModuleCapLoraScreen::showMainMenuScreen()
{
  Template::renderHead("Cap Lora-1262");
  setEntries({
    {"Chatting"},
    {"Lora Setting"},
    {"GPS Data"},
    {"Wardriving"},
  });
}

void ModuleCapLoraScreen::showChatScreen()
{
  //
}

void ModuleCapLoraScreen::initChat()
{
  showChatScreen();
}
