#include "os/screens/nfc/NFCRC522Screen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/nfc/NFCMenuScreen.h"
#include "os/utility/HelperUtility.h"

NFCRC522Screen::~NFCRC522Screen()
{
  _module.PICC_HaltA();
  _module.PCD_StopCrypto1();
  M5Cardputer.Ex_I2C.release();
}

void NFCRC522Screen::init()
{
  M5Cardputer.Ex_I2C.begin();
  Template::renderHead("RC522");
  Template::drawStatusBody("Checking the module...");
  HelperUtility::delayMs(500);
  if (!M5Cardputer.Ex_I2C.scanID(I2C_ADDRESS))
  {
    Template::drawStatusBody("Module not found.");
    HelperUtility::delayMs(1500);
    _global->setScreen(new NFCMenuScreen());
    return;
  }
  _module.PCD_Init();

  goMainMenu();
}


void NFCRC522Screen::goMainMenu()
{
  currentState = STATE_MAIN_MENU;
  Template::renderHead("RC522");
  setEntries({
    {"Scan UID"},
    {"MIFARE Classic"}
  });
}

void NFCRC522Screen::goMifareClassic()
{
  currentState = STATE_MIFARE_CLASSIC;
  Template::renderHead("Mifare Classic");
  setEntries({
  {"Discovered Keys"},
    {"Dump Memory"},
  });
}

bool NFCRC522Screen::resetCardState()
{
  byte req_buff_size=2;
  byte req_buff[req_buff_size];
  _module.PCD_StopCrypto1();
  _module.PICC_HaltA();
  HelperUtility::delayMs(100);
  _module.PICC_WakeupA(req_buff, &req_buff_size);
  HelperUtility::delayMs(100);
  return static_cast<MFRC522_I2C::StatusCode>(_module.PICC_Select( &(_module.uid),0)) != MFRC522_I2C::STATUS_TIMEOUT;
}


void NFCRC522Screen::goShowDiscoveredKeys()
{
  currentState = STATE_SHOW_KEY;
  Template::renderHead("Discovered Keys");
  auto piccType = static_cast<MFRC522_I2C::PICC_Type>(_module.PICC_GetType(_currentCard.sak));
  auto piccName = _module.PICC_GetTypeName(piccType);
  char sakStr[10];
  sprintf(sakStr, "%02X", _currentCard.sak);
  std::vector<ListEntryItem> temporaryKeys = {
    {"Type", std::string(reinterpret_cast<const char*>(piccName))},
    {"UID", cUidToString(_currentCard.uidByte, _currentCard.size)},
    {"SAK", sakStr},
    {"Keys:"}
  };
  const auto cardDetail = _mf1CardDetails.find(piccType);
  for (int sector = 0;sector < cardDetail->second.first;sector++)
  {
    std::string sectorLabel = "Sector " + std::to_string(sector);
    temporaryKeys.push_back({
      _mf1AuthKeys[sector].first.c_str(),
      sectorLabel + "A",
    });
    temporaryKeys.push_back({
      _mf1AuthKeys[sector].second.c_str(),
      sectorLabel + "B",
    });
  }

  setEntries(temporaryKeys);
}

void NFCRC522Screen::callScanUid()
{
  currentState = STATE_SCAN_UID;

  setEntries({});
  Template::renderHead("Scan UID");
  Template::drawStatusBody("Scanning ISO14443...");

  bool isFound = false;
  auto _body = Template::createBody();
  const auto now = millis();
  while (now - millis() < 5000)
  {
    if (_module.PICC_IsNewCardPresent() && _module.PICC_ReadCardSerial())
    {
      const uint8_t piccType = _module.PICC_GetType(_module.uid.sak);
      const auto piccName = _module.PICC_GetTypeName(piccType);

      _body.setTextSize(1);
      _body.drawCenterString(piccName, _body.width() / 2, _body.height() / 2 - _body.fontHeight() * 3);
      _body.setTextSize(2);
      _body.drawCenterString("ISO14443", _body.width() / 2, _body.height() / 2 - _body.fontHeight());
      _body.drawCenterString(cUidToString(_module.uid.uidByte, _module.uid.size).c_str(), _body.width() / 2, _body.height() / 2);

      isFound = true;
      break;
    }
  }

  if (!isFound)
  {
    _body.setTextSize(2);
    _body.drawCenterString("No Tag Found", _body.width() / 2, _body.height() / 2 - _body.fontHeight() / 2);
  }
  _body.setTextSize(1);
  _body.drawCenterString("Enter to scan, Back to menu.", _body.width() / 2, _body.height() - _body.fontHeight());
  Template::renderBody(&_body);
}

void NFCRC522Screen::callAuthenticate()
{
  currentState = STATE_AUTHENTICATE;

  setEntries({});
  Template::renderHead("Authenticate");
  Template::drawStatusBody("Scanning MIFARE Classic...");
  const auto start = millis();
  while (true)
  {
    if (millis() - start > 5000)
    {
      Template::drawStatusBody("No card found");
      HelperUtility::delayMs(1500);
      goMainMenu();
      return;
    }

    if (_module.PICC_IsNewCardPresent() && _module.PICC_ReadCardSerial()) {
      _currentCard.sak = _module.uid.sak;
      _currentCard.size = _module.uid.size;
      memcpy(_currentCard.uidByte, _module.uid.uidByte, _currentCard.size);
      break;
    }
  }

  const auto cardDetail = _mf1CardDetails.find(static_cast<MFRC522_I2C::PICC_Type>(_module.PICC_GetType(_currentCard.sak)));
  if (cardDetail == _mf1CardDetails.end())
  {
    Template::drawStatusBody("No supported tag found");
    HelperUtility::delayMs(1500);
    goMainMenu();
  }

  const std::vector<MFRC522_I2C::PICC_Command> keyList = {
    _module.PICC_CMD_MF_AUTH_KEY_A,
    _module.PICC_CMD_MF_AUTH_KEY_B
  };
  for (int sector = 0;sector < cardDetail->second.first;sector++)
  {
    for (const auto &keyType : keyList)
    {
      std::string sectorProgress = std::to_string(sector) + " / " + std::to_string(cardDetail->second.first - 1);
      if (keyType == _module.PICC_CMD_MF_AUTH_KEY_A)
        Template::drawStatusBody("Auth sector "+sectorProgress+" key A...");
      else
        Template::drawStatusBody("Auth sector "+sectorProgress+" key B...");
      for (const auto& key : NFCUtility::getDefaultKeys())
      {
        const auto kv = key.value();
        MFRC522_I2C::MIFARE_Key currentKey = {
          kv[0], kv[1],
          kv[2], kv[3],
          kv[4], kv[5]
        };
        const int blockIndex = (sector < 32) ? (sector * 4 + 3) : (128 + (sector - 32) * 16 + 15);
        const auto response = static_cast<MFRC522_I2C::StatusCode>(_module.PCD_Authenticate(keyType, blockIndex, &currentKey, &_currentCard));
        const bool isCorrect = (response == MFRC522_I2C::STATUS_OK);
        if (isCorrect)
        {
          if (keyType == _module.PICC_CMD_MF_AUTH_KEY_A)
            _mf1AuthKeys[sector].first = key;
          else
            _mf1AuthKeys[sector].second = key;
          break;
        }

        int counter = 0;
        do
        {
          counter ++;
          HelperUtility::delayMs(500);
          if (counter > 5) {
            goMainMenu();
            Template::drawStatusBody("Fail to reset card.");
            HelperUtility::delayMs(1500);
            return;
          }
        } while (!resetCardState());
      }
    }
  }

  goMifareClassic();
}

void NFCRC522Screen::callMemoryReader()
{
  currentState = STATE_MEMORY_READER;
  Template::renderHead("Memory Reader");
  auto piccType = static_cast<MFRC522_I2C::PICC_Type>(_module.PICC_GetType(_currentCard.sak));
  auto piccName = _module.PICC_GetTypeName(piccType);
  char sakStr[10];
  sprintf(sakStr, "%02X", _currentCard.sak);
  std::vector<ListEntryItem> memoryEntries = {
    {"Type", std::string(reinterpret_cast<const char*>(piccName))},
    {"UID", cUidToString(_currentCard.uidByte, _currentCard.size)},
    {"SAK", sakStr},
    {"Memory:"}
  };

  const auto cardDetail = _mf1CardDetails.find(piccType);
  int lastValidatedSector = -1;
  for (int block = 0;block < cardDetail->second.second;block++)
  {
    byte blockData[18];
    byte blockSize = sizeof(blockData);
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

    if (lastValidatedSector != currentSector)
    {
      lastValidatedSector = currentSector;
      const int trailedBlock = (currentSector < 32) ? (currentSector * 4 + 3) : (128 + (currentSector - 32) * 16 + 15);
      MFRC522_I2C::MIFARE_Key activeKey = {};
      MFRC522_I2C::PICC_Command activeCommand;
      if (_mf1AuthKeys[currentSector].first)
      {
        activeCommand = MFRC522_I2C::PICC_CMD_MF_AUTH_KEY_A;
        auto _kv = _mf1AuthKeys[currentSector].first.value();
        activeKey = { _kv[0], _kv[1], _kv[2], _kv[3], _kv[4], _kv[5] };
      } else
      {
        activeCommand = _module.PICC_CMD_MF_AUTH_KEY_B;
        auto _kv = _mf1AuthKeys[currentSector].first.value();
        activeKey = { _kv[0], _kv[1], _kv[2], _kv[3], _kv[4], _kv[5] };
      }

      MFRC522_I2C::StatusCode status;
      int counter = 0;
      do
      {
        status = static_cast<MFRC522_I2C::StatusCode>(_module.PCD_Authenticate(activeCommand, trailedBlock, &activeKey, &_currentCard));
        if (status != MFRC522_I2C::STATUS_OK)
        {
          resetCardState();
        }

        counter++;
        if (counter > 5)
          break;
      } while (status != MFRC522_I2C::STATUS_OK);

      if (status != MFRC522_I2C::STATUS_OK)
      {
        lastValidatedSector = -1;
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
    }


    const auto isRead = static_cast<MFRC522_I2C::StatusCode>(_module.MIFARE_Read(block, blockData, &blockSize));
    if (isRead != MFRC522_I2C::STATUS_OK)
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

void NFCRC522Screen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MAIN_MENU)
  {
    if (entry.label == "Scan UID")
      callScanUid();
    else if (entry.label == "MIFARE Classic")
      callAuthenticate();
  } else if (currentState == STATE_MIFARE_CLASSIC)
  {
    if (entry.label == "Discovered Keys")
      goShowDiscoveredKeys();
    else if (entry.label == "Dump Memory")
      callMemoryReader();
  }
}

void NFCRC522Screen::onBack()
{
  if (currentState == STATE_MAIN_MENU)
    _global->setScreen(new NFCMenuScreen());
  else if (currentState == STATE_MIFARE_CLASSIC)
  {
    _currentCard = {};
    _mf1AuthKeys.fill({});
    goMainMenu();
  } else if (currentState == STATE_SHOW_KEY || currentState == STATE_MEMORY_READER)
    goMifareClassic();
}

void NFCRC522Screen::onEscape()
{
  _currentCard = {};
  _mf1AuthKeys.fill({});
  _global->setScreen(new MainMenuScreen());
}

void NFCRC522Screen::update()
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
        goMainMenu();
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