//
// Created by L Shaf on 19/10/25.
//

#include <unordered_map>
#include "os/screens/nfc/NFCPN532Screen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/nfc/NFCMenuScreen.h"


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
  delayMicroseconds(500);
  const bool _ok = _module.setNormalMode();
  if (!_ok)
  {
    Template::drawStatusBody("Failed to communicate with PN532.");
    delayMicroseconds(1500);
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
  });
}

void NFCPN532Screen::goPN532KillerMenu()
{
  currentState = STATE_PN532_KILLER;
  Template::renderHead("PN532Killer");
  setEntries({
    {"Scan UID"},
    {"MIFARE Classic Read Memory"}
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

void NFCPN532Screen::callMemoryReader()
{
  currentState = STATE_MEMORY_READER;

  setEntries({});
  Template::drawStatusBody("Scanning ISO14443...");
  _module.setNormalMode();

  const auto hf14result = _module.hf14aScan();
  if (hf14result.uid.empty())
  {
    Template::drawStatusBody("No ISO14443 Tag Found.");
    delayMicroseconds(1500);
    goActualMenu();
    return;
  }

  std::vector<ListEntryItem> memoryEntries = {
    {"UID", hf14result.uid_hex.c_str()},
    {"ATQA", hf14result.atqa_hex.c_str()},
    {"SAK", hf14result.sak_hex.c_str()},
    {"Memory:"}
  };

  _module.resetReaderState();
  const auto uid = hf14result.uid;
  std::unordered_map<int, ExtendedPN532Killer::MfcKey> sectorKeyCache;
  for (int block = 0;block < 16;block++)
  {
    std::vector<uint8_t> blockData;
    Template::drawStatusBody("Reading block " + std::to_string(block) + "...");
    int currentSector = floor(block / 16);
    auto cachedKey = sectorKeyCache.find(currentSector);
    if (cachedKey == sectorKeyCache.end())
    {
      for (const auto& key : _defaultKeys)
      {
        blockData = _module.mf1ReadBlock(uid, block, key);
        if (!blockData.empty())
        {
          sectorKeyCache[currentSector] = key;
          break;
        }
      }

      if (blockData.empty())
        sectorKeyCache[currentSector] = _defaultKeys[0];
    } else
    {
        blockData = _module.mf1ReadBlock(uid, block, cachedKey->second);
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
    } else if (entry.label == "MIFARE Classic Read Memory")
    {
      callMemoryReader();
    }
  }
}

void NFCPN532Screen::onBack()
{
  if (currentState == STATE_MEMORY_READER)
  {
    goActualMenu();
  } else
  {
    _global->setScreen(new NFCMenuScreen());
  }
}

void NFCPN532Screen::onEscape()
{
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
  } else
  {
    ListScreen::update();
  }
}