#include "os/screens/wifi/WifiDeauthDetectorScreen.h"

#include "os/screens/wifi/WifiMenuScreen.h"
#include "os/utility/AudioUtility.h"

void WifiDeauthDetectorScreen::init()
{
  Template::renderHead("Deauth Detector");
  WiFi.mode(WIFI_MODE_STA);
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_rx_cb(&WifiDeauthDetectorScreen::deauthDetectedCallback);
}

void WifiDeauthDetectorScreen::onBack()
{
  _global->setScreen(new WifiMenuScreen());
}

void WifiDeauthDetectorScreen::onEnter(ListEntryItem entry)
{
  // do nothing
}

void WifiDeauthDetectorScreen::update()
{
  if (millis() - lastRender >= 1000)
  {
    currentChannel = (currentChannel % 13) + 1;
    esp_wifi_set_channel(currentChannel, WIFI_SECOND_CHAN_NONE);
    lastRender = millis();

    entries.clear();
    for (auto& it : deauthDetectedList)
    {
      // Remove entries older than window
      if (millis() - it.second.timestamp > windowMs) {
        deauthDetectedList.erase(it.first);
        continue;
      }

      std::string macStr = it.second.ssid;
      if (macStr.empty())
      {
        for (size_t i = 0; i < 6; ++i)
        {
          char buf[4];
          sprintf(buf, "%02X", it.first[i]);
          macStr += buf;
          if (i < 5) macStr += ":";
        }
      }

      macStr += " (" + std::to_string(it.second.counter) + ")";

      std::string timePassed;
      const unsigned long secsAgo = (millis() - it.second.timestamp) / 1000UL;
      if (secsAgo == 0) {
        timePassed = "just now";
      } else {
        timePassed = std::to_string(secsAgo) + "s ago";
      }

      entries.push_back({macStr, timePassed});
    }

    if (!entries.empty())
    {
      currentState = STATE_LISTED;
      if (entries.size() < selectedIndex)
      {
        selectedIndex = static_cast<uint8_t>(entries.size());
        scrollOffset = std::max(0, selectedIndex - visibleCount);
      }
      render();
    } else if (currentState == STATE_LISTED)
    {
      currentState = STATE_NO_LIST;
      Template::renderStatus("Waiting for deauth packets...");
    }
  }

  if (currentState == STATE_LISTED)
    ListScreen::update();
  else
  {
    const auto _keyboard = &M5Cardputer.Keyboard;
    if (_keyboard->isChange() && _keyboard->isPressed())
    {
      if (_keyboard->isKeyPressed(KEY_BACKSPACE) || _keyboard->isKeyPressed('`'))
      {
        playSound();
        onBack();
      }
    }
  }
}

std::unordered_map<
  WifiDeauthDetectorScreen::MacAddr,
  std::string,
  WifiDeauthDetectorScreen::MacHash,
  WifiDeauthDetectorScreen::MacEqual
> WifiDeauthDetectorScreen::ssidList = {};

std::unordered_map<
  WifiDeauthDetectorScreen::MacAddr,
  WifiDeauthDetectorScreen::deauthPacketData,
  WifiDeauthDetectorScreen::MacHash,
  WifiDeauthDetectorScreen::MacEqual
> WifiDeauthDetectorScreen::deauthDetectedList = {};

portMUX_TYPE WifiDeauthDetectorScreen::s_ssidLock = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE WifiDeauthDetectorScreen::s_deauthLock = portMUX_INITIALIZER_UNLOCKED;

void WifiDeauthDetectorScreen::deauthDetectedCallback(void* buf, const wifi_promiscuous_pkt_type_t type)
{
  if (type != WIFI_PKT_MGMT || buf == nullptr) return;
  const auto pkt = static_cast<wifi_promiscuous_pkt_t*>(buf);
  const size_t len = pkt->rx_ctrl.sig_len;

  const uint8_t* payload = pkt->payload;
  const uint8_t fc0 = payload[0];
  const uint8_t fc_type = (fc0 >> 2) & 0x03;
  const uint8_t fc_subtype = (fc0 >> 4) & 0x0F;
  if (fc_type != 0) return;

  if (fc_subtype == 0xC)
  {
    const uint8_t* addr2 = payload + 10; // transmitter addr
    MacAddr mac = {};
    std::memcpy(mac.data(), addr2, 6);

    const unsigned long now = millis();

    portENTER_CRITICAL(&s_deauthLock);
    const auto it = deauthDetectedList.find(mac);
    if (it == deauthDetectedList.end()) {
      deauthPacketData d = {};
      d.timestamp = now;
      d.counter = 1;
      const auto ssidIt = ssidList.find(mac);
      if (ssidIt != ssidList.end()) {
        d.ssid = ssidIt->second;
      }
      deauthDetectedList.emplace(mac, d);
      AudioUtility::playNotification();
    } else {
      ++it->second.counter;
      it->second.timestamp = now;
      if (it->second.ssid.empty())
      {
        const auto ssidIt = ssidList.find(mac);
        if (ssidIt != ssidList.end()) {
          it->second.ssid = ssidIt->second;
        }
      }
    }
    portEXIT_CRITICAL(&s_deauthLock);
  }

  // Beacon (subtype 8) or Probe Response (subtype 5) - parse tagged parameters for SSID
  if (fc_subtype == 8 || fc_subtype == 5) {
    // BSSID in management frames for beacon/probe-resp is addr3 (offset 16)
    if (len < 24 + 12) return; // header + fixed params
    MacAddr bssid = {};
    std::memcpy(bssid.data(), payload + 16, 6);

    size_t pos = 24 + 12; // header (24) + fixed beacon/probe params (12)
    while (pos + 2 <= static_cast<size_t>(len)) {
      const uint8_t elem_id = payload[pos];
      const uint8_t elem_len = payload[pos + 1];
      if (pos + 2 + elem_len > static_cast<size_t>(len)) break; // malformed/truncated
      if (elem_id == 0) { // SSID element
        if (elem_len != 0) {
          portENTER_CRITICAL(&s_ssidLock);
          std::string ssid(reinterpret_cast<const char*>(payload + pos + 2), elem_len);
          const auto f = ssidList.find(bssid);
          if (f == ssidList.end()) ssidList.emplace(bssid, ssid);
          portEXIT_CRITICAL(&s_ssidLock);
        }
        break; // SSID parsed; stop
      }
      pos += 2 + elem_len;
    }
  }
}
