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
  Template::renderStatus("Loading...");
  attacker = new WifiAttackUtility();
  render();
}

void WifiBeaconSpamScreen::broadcastWifiBeacon()
{
  const int ssidLength = static_cast<int>(ssidList.size());
  if (ssidLength == 0) return;
  for (int i = 0; i < ssidLength; i++) {
    const auto currentChannel = static_cast<int>((i / static_cast<float>(ssidLength) * 13) + 1);
    attacker->beacon_spam(ssidList[i], currentChannel);
  }
}


void WifiBeaconSpamScreen::update()
{
  const auto _keyboard = &M5Cardputer.Keyboard;
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
  if (millis() - lastRender >= 500) {
    lastRender = millis();
    auto body = Template::createBody();

    static int dotCount = 0;
    const std::string loadingStr = "[" + std::string(1, loadingBar[dotCount]) + "] Spamming...";
    dotCount = (dotCount + 1) % 4;

    body.drawCenterString(loadingStr.c_str(), body.width() / 2, body.height() / 2 - body.fontHeight() / 2);
    body.drawCenterString("esc/back to stop", body.width() / 2, body.height() - body.fontHeight() - 2);
    Template::renderBody(&body);
  }
}
