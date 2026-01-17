//
// Created by l-sha on 19/10/2025.
//

#include "ExtendedPN532Killer.h"

ExtendedPN532Killer::ExtendedPN532Killer(HardwareSerial &serial)
    : PN532Killer(serial) {}

bool ExtendedPN532Killer::mf1AuthenticateBlock(const uint8_t block, const uint8_t keyType, MfcKey key, std::vector<uint8_t> uid)
{
    if (uid.size() > 4) {
        uid.resize(4);
    }
    std::vector<uint8_t> data = {
        0x01, // MIFARE Classic
        keyType, // Key Type A/B
        block // Block Number
    };

    data.insert(data.end(), key.begin(), key.end());
    data.insert(data.end(), uid.begin(), uid.end());

    const auto resp = writeCommand(PN532KillerCmd::InDataExchange, data);
    return !resp.empty() && resp[0] == 0x00;
}

ExtendedPN532Killer::CardType ExtendedPN532Killer::getCardType(byte sak)
{
    if (sak & 0x04) { // UID not complete
        return CARD_TYPE_NOT_COMPLETE;
    }

    switch (sak) {
    case 0x09:	return CARD_TYPE_MIFARE_MINI;
    case 0x08:	return CARD_TYPE_MIFARE_1K;
    case 0x18:	return CARD_TYPE_MIFARE_4K;
    case 0x00:	return CARD_TYPE_MIFARE_UL;
    case 0x10:
    case 0x11:	return CARD_TYPE_MIFARE_PLUS;
    case 0x01:	return CARD_TYPE_TNP3XXX;
    default:	break;
    }

    if (sak & 0x20) {
        return CARD_TYPE_ISO_14443_4;
    }

    if (sak & 0x40) {
        return CARD_TYPE_ISO_18092;
    }

    return CARD_TYPE_UNKNOWN;
}

std::string ExtendedPN532Killer::getCardTypeStr(byte sak)
{
    const auto cardType = getCardType(sak);
    switch (cardType)
    {
    case CARD_TYPE_MIFARE_MINI: return "MIFARE Mini";
    case CARD_TYPE_MIFARE_1K: return "MIFARE Classic 1K";
    case CARD_TYPE_MIFARE_4K: return "MIFARE Classic 4K";
    case CARD_TYPE_MIFARE_UL: return "MIFARE Ultralight";
    case CARD_TYPE_MIFARE_PLUS: return "MIFARE Plus";
    case CARD_TYPE_TNP3XXX: return "TNP3XXX";
    case CARD_TYPE_ISO_14443_4: return "ISO/IEC 14443-4";
    case CARD_TYPE_ISO_18092: return "ISO/IEC 18092 (NFC)";
    case CARD_TYPE_NOT_COMPLETE: return "UID Not Complete";
    case CARD_TYPE_UNKNOWN:
    default: return "Unknown";
    }
}

std::vector<uint8_t> ExtendedPN532Killer::mf1ReadBlock(const std::vector<uint8_t>& uid, const uint8_t block, MfcKey key)
{
    // compute sector index for authentication tracking
    const int current_sector = (block < 128) ? (block / 4) : static_cast<uint8_t>(((block - 128) / 16) + 32);

    // authenticate if we're not already authenticated for this sector
    if (mf1AuthenticatedSector != current_sector) {
        bool auth_result = mf1AuthenticateBlock(block, MFC_KEY_TYPE_A, key, uid);
        if (!auth_result) {
            mf1AuthenticatedUseKeyA = false;
            auth_result = mf1AuthenticateBlock(block, MFC_KEY_TYPE_B, key, uid);
        }

        if (!auth_result) {
            mf1AuthenticatedUseKeyA = true;
            return {};
        }

        mf1AuthenticatedSector = current_sector;
    }

    // build read command for MIFARE Classic
    const std::vector<uint8_t> data = {
        0x01, // MIFARE Classic target
        0x30, // MIFARE Classic Read command
        block
    };

    // send command and get response bytes
    auto resp = writeCommand(PN532KillerCmd::InDataExchange, data);

    // if we have a full block (>=16 bytes) and the block is a trailer, expose key bytes per auth type
    auto isTrailerBlock = [](uint8_t b)->bool {
        if (b < 128) return (b % 4) == 3;
        return ((b - 128) % 16) == 15;
    };

    if (resp.size() >= 16) {
        if (isTrailerBlock(block)) {
            // MfcKey is assumed to be an array-like container of 6 bytes (e.g. std::array<uint8_t,6>)
            if (mf1AuthenticatedUseKeyA) {
                // Key A occupies bytes 0..5 in trailer; replace those bytes with provided key
                std::vector<uint8_t> parsed;
                parsed.reserve(resp.size());
                parsed.insert(parsed.end(), key.begin(), key.end());                 // key (6 bytes)
                if (resp.size() > 6) parsed.insert(parsed.end(), resp.begin() + 6, resp.end());
                resp = std::move(parsed);
            } else {
                // Key B occupies bytes 10..15 in trailer; keep first 10 bytes and append key
                std::vector<uint8_t> parsed;
                parsed.reserve(resp.size());
                parsed.insert(parsed.end(), resp.begin(), resp.begin() + std::min<size_t>(10, resp.size()));
                parsed.insert(parsed.end(), key.begin(), key.end());
                resp = std::move(parsed);
            }
        }
    }

    return resp;
}