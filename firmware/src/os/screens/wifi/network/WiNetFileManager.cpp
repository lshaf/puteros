//
// Created by l-sha on 08/12/2025.
//

#include "WiNetFileManager.h"

void WiNetFileManager::init()
{
  Template::renderHead("Web File Manager");
  if (!_global->getIsSDCardLoaded())
  {
    Template::renderStatus("SD Card is not loaded", TFT_RED);
    HelperUtility::delayMs(2000);
    return;
  }
}

void WiNetFileManager::onBack()
{
  if (STATE_MENU == currentState)
  {
    server.stop();
  }
}

void WiNetFileManager::onEnter(ListEntryItem entry)
{
  switch (currentState)
  {
    case STATE_MENU:
    {
      if (entry.title == "Web File Manager")
      {
        currentState = STATE_WEB_MANAGER;
        WiNetFileManagerScreen fileManagerScreen;
        fileManagerScreen.startWebServer(server);
      }
      break;
    }
    default:
      break;
  }
}