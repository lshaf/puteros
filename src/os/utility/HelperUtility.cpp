//
// Created by l-sha on 09/10/2025.
//

#include <Arduino.h>
#include "os/utility/HelperUtility.h"

std::string HelperUtility::stringToHex(const std::string& input)
{
  std::string hex;
  for (size_t i = 0; i < input.size(); ++i) {
    char buf[3];
    snprintf(buf, sizeof(buf), "%02X", static_cast<unsigned char>(input[i]));
    hex += buf;
  }
  return hex;
}

void HelperUtility::delayMicroseconds(unsigned long us)
{
  const unsigned long start = micros();
  while (micros() - start < us) {}
}