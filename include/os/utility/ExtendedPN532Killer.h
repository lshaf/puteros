//
// Created by l-sha on 19/10/2025.
//

#pragma once
#include "PN532Killer.h"

class ExtendedPN532Killer final : public PN532Killer
{
public:
  using MfcKey = std::array<uint8_t, 6>;
  int8_t mf1AuthenticatedSector = -1;
  bool mf1AuthenticatedUseKeyA = true;
  uint8_t MFC_KEY_TYPE_A = 0x60;
  uint8_t MFC_KEY_TYPE_B = 0x61;

  explicit ExtendedPN532Killer(HardwareSerial &serial);
  ~ExtendedPN532Killer() override = default;

  bool mf1AuthenticateBlock(uint8_t block, uint8_t keyType, MfcKey key, std::vector<uint8_t> uid);
  std::vector<uint8_t> mf1ReadBlock(const std::vector<uint8_t>& uid, uint8_t block, MfcKey key);
  void resetReaderState()
  {
    mf1AuthenticatedSector = -1;
    mf1AuthenticatedUseKeyA = true;
  }
};
