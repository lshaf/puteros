//
// Created by l-sha on 30/10/2025.
//

#pragma once

#include <M5Cardputer.h>

class AudioUtility
{
public:
  static std::vector<int> c_major_scale;
  static void playRandomTone(int semitoneShift = 0, double durationSec = 0.15);
  static void playNotification();
};