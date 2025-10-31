//
// Created by l-sha on 31/10/2025.
//

#pragma once
#include "os/core/ListScreen.hpp"

class UtilityQRCodeScreen final : public ListScreen
{
public:
  void init() override;

protected:
  void onEnter(ListEntryItem entry) override;
  void onBack() override;
  void onEscape() override;
  void refreshOption();

private:
  bool isInverted = false;
  enum State_e
  {
    STATE_MENU,
    STATE_QRCODE
  } currentState = STATE_MENU;
  enum Mode_e
  {
    MODE_WRITE,
    MODE_FILE,
  } currentMode = MODE_WRITE;

  std::string qrPath = "/puteros/qrcode";
  bool generateQRCode(const std::string& data);
  void generateFolder();
};
