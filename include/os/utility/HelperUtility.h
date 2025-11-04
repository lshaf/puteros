//
// Created by l-sha on 09/10/2025.
//

#pragma once

#include <M5GFX.h>
#include <string>

class HelperUtility
{
public:
  static std::string stringToHex(const std::string& input);
  static void delayMs(unsigned long us);
  static void drawWrappedCenterString(M5Canvas &canvas, const std::string &text);
  static void makeDirectoryRecursive(const std::string& path);
};