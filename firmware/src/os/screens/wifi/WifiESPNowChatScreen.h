//
// Created by l-sha on 29/10/2025.
//

#pragma once

#include "os/core/ScreenState.hpp"
#include "os/component/Template.hpp"

class WifiESPNowChatScreen final : public ScreenState
{
public:
  WifiESPNowChatScreen();
  ~WifiESPNowChatScreen() override;

  void init() override;
  void update() override;
  void render() override;

  void sendMessage(const std::string& message);

  static WifiESPNowChatScreen* instance;
  void onDataRecv(const uint8_t *mac_addr, const uint8_t *data, int data_len);
  static void onDataRecvCb(const uint8_t *mac_addr, const uint8_t *data, int data_len)
  {
    if (instance) instance->onDataRecv(mac_addr, data, data_len);
  }
protected:
  enum State_e
  {
    STATE_CHAT,
    STATE_COMPOSE,
  } currentState = STATE_CHAT;
  char DEFAULT_CODE[5] = {0x00, 0x01, 0x14, 0x25, 0x36};
  uint8_t BROADCAST_ADDRESS[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
  struct Message_s
  {
    char code[5];
    uint8_t nameLength;
    char name[14];
    uint8_t messageLength;
    char text[224];
  };

  struct ReceivedMessage_s
  {
    uint8_t sender[6];
    Message_s message;
  };


  std::vector<ReceivedMessage_s> messageQueue;
};