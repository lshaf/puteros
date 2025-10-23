#include "os/screens/nfc/NFCRC522Screen.h"
#include "os/screens/MainMenuScreen.hpp"
#include "os/screens/nfc/NFCMenuScreen.h"
#include "os/utility/HelperUtility.h"

#include <Wire.h>

NFCRC522Screen::~NFCRC522Screen()
{
  _module->PICC_HaltA();
  _module->PCD_StopCrypto1();
  M5Cardputer.Ex_I2C.release();
}

void NFCRC522Screen::init()
{
  M5Cardputer.Ex_I2C.begin();
  _module = new MFRC522_I2C(0x28, -1);
  Template::renderHead("NFC RC522");
  Template::drawStatusBody("Checking the module...");
  HelperUtility::delayMs(500);
  if (!M5Cardputer.Ex_I2C.scanID(I2C_ADDRESS))
  {
    Template::drawStatusBody("Module not found.");
    HelperUtility::delayMs(1500);
    _global->setScreen(new NFCMenuScreen());
    return;
  }
  _module->PCD_Init();

  goMainMenu();
}


void NFCRC522Screen::goMainMenu()
{
  currentState = STATE_MAIN_MENU;
  setEntries({
    {"Scan UID"},
    {"MIFARE Classic"}
  });
}

void NFCRC522Screen::callScanUid()
{
  currentState = STATE_SCAN_UID;

  setEntries({});
  Template::drawStatusBody("Scanning ISO14443...");

  bool isFound = false;
  auto _body = Template::createBody();
  const auto now = millis();
  while (now - millis() < 5000)
  {
    if (_module->PICC_IsNewCardPresent() && _module->PICC_ReadCardSerial())
    {
      std::string uidStr = "";
      for (byte i = 0; i < _module->uid.size; i++)
      {
        char line[3];
        sprintf(line, "%02X", _module->uid.uidByte[i]);
        uidStr += line;
      }

      const uint8_t piccType = _module->PICC_GetType(_module->uid.sak);
      const auto piccName = _module->PICC_GetTypeName(piccType);

      _body.setTextSize(1);
      _body.drawCenterString(piccName, _body.width() / 2, _body.height() / 2 - _body.fontHeight() * 3);
      _body.setTextSize(2);
      _body.drawCenterString("ISO14443", _body.width() / 2, _body.height() / 2 - _body.fontHeight());
      _body.drawCenterString(uidStr.c_str(), _body.width() / 2, _body.height() / 2);

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

void NFCRC522Screen::onEnter(ListEntryItem entry)
{
  if (currentState == STATE_MAIN_MENU)
  {
    if (entry.label == "Scan UID")
      callScanUid();
  }
}

void NFCRC522Screen::onBack()
{
  _global->setScreen(new NFCMenuScreen());
}

void NFCRC522Screen::onEscape()
{
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