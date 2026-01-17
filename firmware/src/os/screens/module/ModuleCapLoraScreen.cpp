//
// Created by L Shaf on 2026-01-17.
//

#include "ModuleCapLoraScreen.h"
#include "ModuleMenuScreen.h"

bool ModuleCapLoraScreen::isLoraAttached()
{
  _global->loadLora();
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));

  // Try to read any register multiple times
  for (uint8_t reg = 0x00; reg < 0x10; reg += 0x10) {
    SPI.transfer(reg & 0x7F);  // Read command
    uint8_t response = SPI.transfer(0x00);

    // Check if response is not all 0x00 or 0xFF (common when no device)
    if (response != 0x00 && response != 0xFF) {
      SPI.endTransaction();
      return true;
    }
  }
  SPI.endTransaction();
  return false;
}

void ModuleCapLoraScreen::init()
{
  Template::renderHead("Cap Lora-1262");
  Serial1.begin(115200, SERIAL_8N1, 15);
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
      showGSPDataScreen();
    } else if (entry.label == "Wardriving")
    {}
  }
}

void ModuleCapLoraScreen::onBack()
{
  Serial1.flush();
  while (Serial1.available()) Serial1.read();
  Serial1.end();
  _global->loadSDCard();
  _global->setScreen(new ModuleMenuScreen());
}

void ModuleCapLoraScreen::update()
{
  while (Serial1.available()) gps.encode(Serial1.read());
  ListScreen::update();
}

void ModuleCapLoraScreen::showGSPDataScreen()
{
  currentState = STATE_GPS_DATA;
  auto body = Template::createBody();
  body.setTextSize(1);
  body.setTextColor(TFT_WHITE);
  body.println(("LAT: " + String(gps.location.lat(), 9)).c_str());
  body.println(("LNG: " + String(gps.location.lng(), 9)).c_str());
  body.println(("SPEED: " + String(gps.speed.kmph(), 2) + " km/h").c_str());
  body.println(("COURSE: " + String(gps.course.deg(), 2) + " deg").c_str());
  body.println(("ALTITUDE: " + String(gps.altitude.meters(), 2) + " m").c_str());
  body.println(("SATELLITES: " + String(gps.satellites.value())).c_str());
  body.println(("DATE: " + getCurrentGPSDate()).c_str());
  body.println(("TIME: " + getCurrentGPSTime() + " UTC").c_str());
  Template::renderBody(&body);
}

void ModuleCapLoraScreen::showMainMenuScreen()
{
  currentState = STATE_MENU;
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
  const int state = lora.begin(
    LORA_FREQ, LORA_BW, LORA_SF, LORA_CR,
    LORA_SYNC_WORD, LORA_TX_POWER, LORA_PREAMBLE_LEN
  );
  if (state != RADIOLIB_ERR_NONE)
  {
    Template::renderStatus("Lora init failed", TFT_RED);
    HelperUtility::delayMs(2000);
    showMainMenuScreen();
    return;
  }

  showChatScreen();
}
