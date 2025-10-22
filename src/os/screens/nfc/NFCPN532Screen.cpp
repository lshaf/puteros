//
// Created by L Shaf on 19/10/25.
//

#include "os/screens/nfc/NFCPN532Screen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/nfc/NFCMenuScreen.h"
#include "os/utility/HelperUtility.h"


NFCPN532Screen::~NFCPN532Screen()
{
  Serial1.flush();
  while (Serial1.available()) Serial1.read();
  Serial1.end();
}

void NFCPN532Screen::init()
{
  Serial1.begin(115000, SERIAL_8N1, 2, 1);
  Template::drawStatusBody("Check PN532 module...");
  HelperUtility::delayMicroseconds(500);
  const bool _ok = _module.setNormalMode();
  if (!_ok)
  {
    Template::drawStatusBody("Failed to communicate with PN532.");
    HelperUtility::delayMicroseconds(1500);
    onEscape();
    return;
  }

  _isKiller = _module.isPn532killer();
  goActualMenu();
}

void NFCPN532Screen::goPN532Menu()
{
  currentState = STATE_PN532;
  Template::renderHead("PN532");
  setEntries({
    {"Scan UID"},
    {"MIFARE Classic"}
  });
}

void NFCPN532Screen::goPN532KillerMenu()
{
  currentState = STATE_PN532_KILLER;
  Template::renderHead("PN532Killer");
  setEntries({
    {"Scan UID"},
    {"MIFARE Classic"}
  });
}

void NFCPN532Screen::goActualMenu()
{
  if (_isKiller)
  {
    goPN532KillerMenu();
  } else
  {
    goPN532Menu();
  }
}

void NFCPN532Screen::callScanUid()
{
  currentState = STATE_SCAN_UID;

  setEntries({});
  Template::drawStatusBody("Scanning ISO14443...");
  auto _body = Template::createBody();
  bool isFound = false;
  _module.setNormalMode();
  const auto hf14result = _module.hf14aScan();
  if (!hf14result.uid.empty())
  {
    isFound = true;
    _body.setTextSize(1);
    _body.drawCenterString(_module.getCardTypeStr(hf14result.sak).c_str(), _body.width() / 2, _body.height() / 2 - _body.fontHeight() * 3);
    _body.setTextSize(2);
    _body.drawCenterString("ISO14443", _body.width() / 2, _body.height() / 2 - _body.fontHeight());
    _body.drawCenterString(hf14result.uid_hex.c_str(), _body.width() / 2, _body.height() / 2);
  }

  if (!isFound && _isKiller)
  {
    Template::drawStatusBody("Scanning ISO15693...");
    const auto hf15result = _module.hf15Scan();
    if (!hf15result.uid.empty())
    {
      isFound = true;
      _body.setTextSize(1);
      _body.drawCenterString(_module.getCardTypeStr(hf14result.sak).c_str(), _body.width() / 2, _body.height() / 2 - _body.fontHeight() * 3);
      _body.setTextSize(2);
      _body.drawCenterString("ISO15693", _body.width() / 2, _body.height() / 2 - _body.fontHeight());
      _body.drawCenterString(hf15result.uid_hex.c_str(), _body.width() / 2, _body.height() / 2);
    }
  }

  _body.setTextSize(1);
  if (!isFound) _body.print("No Tag Found.");
  _body.drawCenterString("Enter to scan, Back to menu.", _body.width() / 2, _body.height() - _body.fontHeight());
  Template::renderBody(&_body);
}

std::string NFCPN532Screen::convertKeyToString(const NullableMfcKey& key)
{
  if (!key) return "??:??:??:??:??:??";
  char buffer[20];
  const auto _kv = key.value();
  sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X",
    _kv[0], _kv[1],
    _kv[2], _kv[3],
    _kv[4], _kv[5]
  );

  return buffer;
}

void NFCPN532Screen::goShowDiscoveredKeys()
{
  currentState = STATE_SHOW_KEY;
  std::vector<ListEntryItem> temporaryKeys = {
    {"Card", _module.getCardTypeStr(_currentCard.sak)},
    {"UID", _currentCard.uid_hex.c_str()},
    {"ATQA", _currentCard.atqa_hex.c_str()},
    {"SAK", _currentCard.sak_hex.c_str()},
    {"Keys:"}
  };
  const auto cardDetail = _mf1CardDetails.find(_module.getCardType(_currentCard.sak));
  for (int sector = 0;sector < cardDetail->second.first;sector++)
  {
    std::string sectorLabel = "Sector " + std::to_string(sector);
    temporaryKeys.push_back({
      convertKeyToString(_mf1AuthKeys[sector].first),
      sectorLabel + "A",
    });
    temporaryKeys.push_back({
      convertKeyToString(_mf1AuthKeys[sector].second),
      sectorLabel + "A",
    });
  }

  setEntries(temporaryKeys);
}


void NFCPN532Screen::goMifareClassicMenu()
{
  currentState = STATE_MIFARE_CLASSIC;
  setEntries({
    {"Discovered Keys"},
    {"Dump Memory"}
  });
}

void NFCPN532Screen::callAuthenticate()
{
  currentState = STATE_AUTHENTICATE;

  setEntries({});
  Template::drawStatusBody("Scanning ISO14443...");
  _module.setNormalMode();

  _currentCard = _module.hf14aScan();
  if (_currentCard.uid.empty())
  {
    Template::drawStatusBody("No ISO14443 Tag Found.");
    HelperUtility::delayMicroseconds(1500);
    goActualMenu();
    return;
  }

  const auto cardType = _module.getCardType(_currentCard.sak);
  const auto cardDetail = _mf1CardDetails.find(cardType);
  if (cardDetail == _mf1CardDetails.end())
  {
    Template::drawStatusBody("No Supported Tag Found.");
    HelperUtility::delayMicroseconds(1500);
    goActualMenu();
    return;
  }

  std::vector<uint8_t> keyList = {
    _module.MFC_KEY_TYPE_A,
    _module.MFC_KEY_TYPE_B
  };
  for (int sector = 0;sector < cardDetail->second.first;sector++)
  {
    for (const auto &keyType : keyList)
    {
      std::string sectorProgress = std::to_string(sector) + " / " + std::to_string(cardDetail->second.first - 1);
      if (keyType == _module.MFC_KEY_TYPE_A)
        Template::drawStatusBody("Auth sector "+sectorProgress+" key A...");
      else
        Template::drawStatusBody("Auth sector "+sectorProgress+" key B...");
      for (const auto& key : _defaultKeys)
      {
        const int blockIndex = (sector < 32) ? (sector * 4 + 3) : (128 + (sector - 32) * 16 + 15);
        const auto isCorrect = _module.mf1AuthenticateBlock(blockIndex, keyType, key, _currentCard.uid);
        if (isCorrect)
        {
          if (keyType == _module.MFC_KEY_TYPE_A)
            _mf1AuthKeys[sector].first = key;
          else
            _mf1AuthKeys[sector].second = key;
          break;
        }
      }
    }
  }

  goMifareClassicMenu();
}

void NFCPN532Screen::callMemoryReader()
{
  currentState = STATE_MEMORY_READER;
  std::vector<ListEntryItem> memoryEntries = {
    {"Card", _module.getCardTypeStr(_currentCard.sak)},
    {"UID", _currentCard.uid_hex.c_str()},
    {"ATQA", _currentCard.atqa_hex.c_str()},
    {"SAK", _currentCard.sak_hex.c_str()},
    {"Memory:"}
  };

  _module.resetReaderState();
  const auto cardDetail = _mf1CardDetails.find(_module.getCardType(_currentCard.sak));
  const auto uid = _currentCard.uid;
  for (int block = 0;block < cardDetail->second.second;block++)
  {
    std::vector<uint8_t> blockData;
    std::string blockProgress = std::to_string(block) + " / " + std::to_string(cardDetail->second.second - 1);
    Template::drawStatusBody("Reading block " + blockProgress + "...");
    const int currentSector = (block < 128) ? (block / 4) : static_cast<uint8_t>(((block - 128) / 16) + 32);
    if (!_mf1AuthKeys[currentSector].first && !_mf1AuthKeys[currentSector].second)
    {
      memoryEntries.push_back({
        "??:??:??:??:??:??:??:??",
        "Block " + std::to_string(block)
      });
      memoryEntries.push_back({
        "??:??:??:??:??:??:??:??",
        "Block " + std::to_string(block)
      });
      continue;
    }

    if (_mf1AuthKeys[currentSector].first)
    {
      blockData = _module.mf1ReadBlock(uid, block, _mf1AuthKeys[currentSector].first.value());
    }
    if (blockData.empty() and _mf1AuthKeys[currentSector].second)
    {
      blockData = _module.mf1ReadBlock(uid, block, _mf1AuthKeys[currentSector].second.value());
    }

    if (blockData.empty())
    {
      memoryEntries.push_back({
        "??:??:??:??:??:??:??:??",
        "Block " + std::to_string(block)
      });
      memoryEntries.push_back({
        "??:??:??:??:??:??:??:??",
        "Block " + std::to_string(block)
      });
    } else
    {
      char buffer[50];
      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
        blockData[0], blockData[1], blockData[2], blockData[3],
        blockData[4], blockData[5], blockData[6], blockData[7]
      );
      memoryEntries.push_back({
        buffer,
        "Block " + std::to_string(block)
      });

      sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X:%02X:%02X",
        blockData[8], blockData[9], blockData[10], blockData[11],
        blockData[12], blockData[13], blockData[14], blockData[15]
      );
      memoryEntries.push_back({
        buffer,
        "Block " + std::to_string(block)
      });
    }
  }

  setEntries(memoryEntries);
}

void NFCPN532Screen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_PN532_KILLER || currentState == STATE_PN532)
  {
    if (entry.label == "Scan UID")
    {
      callScanUid();
    } else if (entry.label == "MIFARE Classic")
    {
      callAuthenticate();
    }
  } else if (currentState == STATE_MIFARE_CLASSIC)
  {
    if (entry.label == "Discovered Keys")
      goShowDiscoveredKeys();
    else if (entry.label == "Dump Memory")
      callMemoryReader();
  }
}

void NFCPN532Screen::onBack()
{
  if (currentState == STATE_MIFARE_CLASSIC)
  {
    _currentCard = {};
    _mf1AuthKeys.fill({});
    goActualMenu();
  } else if (currentState == STATE_MEMORY_READER || currentState == STATE_SHOW_KEY)
  {
    goMifareClassicMenu();
  } else
  {
    _global->setScreen(new NFCMenuScreen());
  }
}

void NFCPN532Screen::onEscape()
{
  _currentCard = {};
  _mf1AuthKeys.fill({});
  _global->setScreen(new MainMenuScreen());
}

void NFCPN532Screen::update()
{
  if (currentState == STATE_SCAN_UID)
  {
    const auto _keyboard = &M5Cardputer.Keyboard;
    if (_keyboard->isChange() && _keyboard->isPressed())
    {
      if (_keyboard->isKeyPressed(KEY_ENTER))
      {
        callScanUid();
      } else if (_keyboard->isKeyPressed(KEY_BACKSPACE))
      {
        goActualMenu();
      } else if (_keyboard->isKeyPressed('`'))
      {
        onEscape();
      }
    }
  } else if (currentState == STATE_AUTHENTICATE)
  {
    // does nothing or uninterruptible
  }else
  {
    ListScreen::update();
  }
}