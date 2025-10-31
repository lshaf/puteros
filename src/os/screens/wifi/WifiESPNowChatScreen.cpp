#include <WiFi.h>
#include "os/screens/wifi/WifiESPNowChatScreen.h"
#include "os/utility/SoundNotification.h"

#include "os/component/InputScreen.hpp"
#include "os/screens/wifi/WifiMenuScreen.h"
#include "os/utility/AudioUtility.h"

WifiESPNowChatScreen* WifiESPNowChatScreen::instance = nullptr;

WifiESPNowChatScreen::WifiESPNowChatScreen()
{
  instance = this;
}

WifiESPNowChatScreen::~WifiESPNowChatScreen()
{
  esp_now_unregister_recv_cb();
  esp_now_deinit();
  instance = nullptr;
}

void WifiESPNowChatScreen::init()
{
  Template::renderHead("ESPNOW Chat");
  Template::renderStatus("Preparing the chat");

  WiFi.mode(WIFI_MODE_STA);
  if (esp_now_init() != ESP_OK)
  {
    Template::renderStatus("Error initializing ESP-NOW", TFT_RED);
    HelperUtility::delayMs(1500);
    _global->setScreen(new WifiMenuScreen());
    return;
  }

  esp_now_register_recv_cb(&WifiESPNowChatScreen::onDataRecvCb);
  if (!esp_now_is_peer_exist(BROADCAST_ADDRESS))
  {
    esp_now_peer_info_t peerInfo = {};
    memcpy(peerInfo.peer_addr, BROADCAST_ADDRESS, 6);
    peerInfo.channel = 1;
    peerInfo.encrypt = false;

    const auto addPeerResult = esp_now_add_peer(&peerInfo);
    if (addPeerResult != ESP_OK){
      switch (addPeerResult)
      {
        case ESP_ERR_ESPNOW_NOT_INIT:
          Template::renderStatus("ESP-NOW not initialized", TFT_RED);
          break;
        case ESP_ERR_ESPNOW_ARG:
          Template::renderStatus("Invalid argument", TFT_RED);
          break;
        case ESP_ERR_ESPNOW_FULL:
          Template::renderStatus("Peer list full", TFT_RED);
          break;
        case ESP_ERR_ESPNOW_NO_MEM:
          Template::renderStatus("Out of memory", TFT_RED);
          break;
        case ESP_ERR_ESPNOW_EXIST:
          Template::renderStatus("Peer already exists", TFT_RED);
          break;
        default:
          Template::renderStatus("Failed to add peer", TFT_RED);
          break;
      }
      HelperUtility::delayMs(1500);
      _global->setScreen(new WifiMenuScreen());
    }
  }
}

void WifiESPNowChatScreen::update()
{
  const auto _keyboard = &M5Cardputer.Keyboard;
  if (_keyboard->isChange() && _keyboard->isPressed())
  {
    if (_keyboard->isKeyPressed(KEY_ENTER))
    {
      currentState = STATE_COMPOSE;
      const std::string message = InputScreen::popup("Message");
      if (!message.empty()) sendMessage(message);
      else render();
    } else if (_keyboard->isKeyPressed('`'))
    {
      _global->setScreen(new WifiMenuScreen());
    } else if (_keyboard->isKeyPressed('p'))
    {
      sendMessage("!ping");
    }
  }
}

void WifiESPNowChatScreen::render()
{
  currentState = STATE_CHAT;
  const int inputY = 14;
  auto body = Template::createBody();
  body.drawRoundRect(0, body.height() - inputY, body.width(), inputY, 2.5, BLUE);
  body.drawString("Press enter to send message", 4, body.height() - inputY + (inputY - body.fontHeight()) / 2);

  int y = body.height() - inputY - 2;
  const float availableCharPerLine = body.width() / body.fontWidth();
  for (int i = messageQueue.size() - 1; i >= 0; i--)
  {
    const auto& msg = messageQueue[i];
    char senderBuf[18];
    snprintf(senderBuf, sizeof(senderBuf), "%02X:%02X:%02X:%02X:%02X:%02X",
             msg.sender[0], msg.sender[1], msg.sender[2],
             msg.sender[3], msg.sender[4], msg.sender[5]);

    const auto pText = std::string(msg.message.text, msg.message.messageLength);
    const auto pName = std::string(msg.message.name, msg.message.nameLength);
    const int msgLine = std::ceil(msg.message.messageLength / availableCharPerLine) + 1;
    y = y - body.fontHeight() * msgLine - 2;
    body.setCursor(0, y);
    body.printf("%s %s:\n%s", senderBuf, pName.c_str(), pText.c_str());

    if (y < 0) break;
  }

  Template::renderHead("ESPNOW Chat");
  Template::renderBody(&body);
}

void WifiESPNowChatScreen::onDataRecv(const uint8_t* mac_addr, const uint8_t* data, int data_len)
{
  if (data_len < 20) return;
  const auto msg = reinterpret_cast<const Message_s*>(data);
  if (memcmp(msg->code, DEFAULT_CODE, sizeof(DEFAULT_CODE)) != 0) return;

  if (messageQueue.size() > 9) messageQueue.erase(messageQueue.begin());
  ReceivedMessage_s receivedMsg;
  memcpy(receivedMsg.sender, mac_addr, 6);
  memcpy(&receivedMsg.message, msg, sizeof(Message_s));
  messageQueue.push_back(receivedMsg);
  AudioUtility::playNotification();
  if (memcmp(msg->text, "!ping", 5) == 0) sendMessage("!pong");
  if (currentState == STATE_CHAT) render();
}

void WifiESPNowChatScreen::sendMessage(const std::string& message)
{
  const auto name = _config->get(APP_CONFIG_DEVICE_NAME, APP_CONFIG_DEVICE_NAME_DEFAULT);
  Message_s msg = {};
  msg.nameLength = sizeof(msg.name);
  msg.messageLength = message.length();
  memcpy(msg.code, DEFAULT_CODE, sizeof(DEFAULT_CODE));
  strncpy(msg.name, name.c_str(), msg.nameLength);
  strncpy(msg.text, message.c_str(), msg.messageLength);

  const uint8_t* target = BROADCAST_ADDRESS;
  const esp_err_t success = esp_now_send(target, reinterpret_cast<uint8_t*>(&msg), sizeof(msg));
  if (success != ESP_OK)
  {
    switch (success)
    {
    case ESP_ERR_ESPNOW_NOT_INIT:
      Template::renderStatus("ESP-NOW not initialized", TFT_RED);
      break;
    case ESP_ERR_ESPNOW_ARG:
      Template::renderStatus("Invalid argument", TFT_RED);
      break;
    case ESP_ERR_ESPNOW_INTERNAL:
      Template::renderStatus("Internal error", TFT_RED);
      break;
    case ESP_ERR_ESPNOW_NO_MEM:
      Template::renderStatus("Out of memory", TFT_RED);
      break;
    case ESP_ERR_ESPNOW_NOT_FOUND:
      Template::renderStatus("Peer not found", TFT_RED);
      break;
    case ESP_ERR_ESPNOW_IF:
      Template::renderStatus("Interface error", TFT_RED);
      break;
    default:
      Template::renderStatus("Failed to send message", TFT_RED);
      break;
    }
    HelperUtility::delayMs(1500);
  } else
  {
    ReceivedMessage_s receivedMsg;
    memcpy(receivedMsg.sender, BROADCAST_ADDRESS, 6);
    memcpy(&receivedMsg.message, &msg, sizeof(Message_s));
    if (messageQueue.size() > 9) messageQueue.erase(messageQueue.begin());
    messageQueue.push_back(receivedMsg);
  }

  render();
}
