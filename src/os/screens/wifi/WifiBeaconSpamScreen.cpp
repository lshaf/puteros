#include "os/screens/wifi/WifiBeaconSpamScreen.h"

#include "os/component/Template.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"

WifiBeaconSpamScreen::~WifiBeaconSpamScreen()
{
  if (attacker != nullptr)
  {
    delete attacker;
    attacker = nullptr;
  }

  ssidList.clear();
}

void WifiBeaconSpamScreen::init()
{
  Template::renderHead("WiFi Spam");
  attacker = new WifiAttackUtility();
  render();

  for (int i = 0; i < 30; i++) {
    ssidList.push_back(generateRandomSSID(15));
  }
}

std::string WifiBeaconSpamScreen::generateRandomSSID(uint8_t length)
{
  std::string out;
  out.reserve(length);
  const uint8_t charset[] = "abcdefghijklmnopqrstuvwxyz0123456789";
  const uint8_t charset_length = sizeof(charset);
  while (out.size() < length) {
    const uint8_t r = static_cast<uint8_t>(random(charset_length));
    out.push_back(charset[r]);
  }
  return out;
}

void WifiBeaconSpamScreen::broadcastWifiBeacon()
{
  currentChannel = (currentChannel % 14) + 1;
  for (int i = 0; i < ssidList.size(); i++) {
    attacker->beacon_spam(ssidList[i], currentChannel);
  }
}


void WifiBeaconSpamScreen::update()
{
  auto _keyboard = &M5Cardputer.Keyboard;
  if (_keyboard->isChange() && _keyboard->isPressed()) {
    if (_keyboard->isKeyPressed('`') || _keyboard->isKeyPressed(KEY_BACKSPACE)) {
      _global->setScreen(new WifiMenuScreen());
      return;
    }
  }

  broadcastWifiBeacon();
  render();
}

void WifiBeaconSpamScreen::render()
{
  if (millis() - lastRender >= 1000) {
    lastRender = millis();
    loading = (loading + 1) % 5;
    auto body = Template::createBody();

    std::string loadingStr = "Spamming";
    for (uint8_t i = 0; i < loading; i++) {
      loadingStr += ".";
    }

    body.setTextSize(2);
    body.drawCenterString(loadingStr.c_str(), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
    body.setTextSize(1);
    body.drawCenterString("esc/back to stop", body.width() / 2, body.height() - body.fontHeight() - 2);
    Template::renderBody(&body);
  }
}
