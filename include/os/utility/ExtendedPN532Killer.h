//
// Created by l-sha on 19/10/2025.
//

#pragma once
#include <Arduino.h>

#include "PN532Killer.h"

class ExtendedPN532Killer final : public PN532Killer
{
public:
  using MfcKey = std::array<uint8_t, 6>;
  int mf1AuthenticatedSector = -1;
  bool mf1AuthenticatedUseKeyA = true;
  uint8_t MFC_KEY_TYPE_A = 0x60;
  uint8_t MFC_KEY_TYPE_B = 0x61;

  enum CardType
  {
    CARD_TYPE_UNKNOWN		= 0,
    CARD_TYPE_ISO_14443_4	= 1,	// CARD compliant with ISO/IEC 14443-4
    CARD_TYPE_ISO_18092		= 2, 	// CARD compliant with ISO/IEC 18092 (NFC)
    CARD_TYPE_MIFARE_MINI	= 3,	// MIFARE Classic protocol, 320 bytes
    CARD_TYPE_MIFARE_1K		= 4,	// MIFARE Classic protocol, 1KB
    CARD_TYPE_MIFARE_4K		= 5,	// MIFARE Classic protocol, 4KB
    CARD_TYPE_MIFARE_UL		= 6,	// MIFARE Ultralight or Ultralight C
    CARD_TYPE_MIFARE_PLUS	= 7,	// MIFARE Plus
    CARD_TYPE_TNP3XXX		= 8,	// Only mentioned in NXP AN 10833 MIFARE Type Identification Procedure
    CARD_TYPE_NOT_COMPLETE	= 255
  };

  explicit ExtendedPN532Killer(HardwareSerial &serial);
  ~ExtendedPN532Killer() override = default;

  CardType getCardType(byte sak);
  std::string getCardTypeStr(byte sak);
  bool mf1AuthenticateBlock(uint8_t block, uint8_t keyType, MfcKey key, std::vector<uint8_t> uid);
  std::vector<uint8_t> mf1ReadBlock(const std::vector<uint8_t>& uid, uint8_t block, MfcKey key);
  void resetReaderState()
  {
    mf1AuthenticatedSector = -1;
    mf1AuthenticatedUseKeyA = true;
  }
};
