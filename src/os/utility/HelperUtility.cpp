//
// Created by l-sha on 09/10/2025.
//

#include <SD.h>
#include <Arduino.h>
#include "os/utility/HelperUtility.h"

void HelperUtility::makeDirectoryRecursive(const std::string& path)
{
  if (SD.exists(path.c_str())) return;
  const auto pathStr = String(path.c_str());
  const int lastSlash = pathStr.lastIndexOf('/');
  if (lastSlash > 0) {
    const String folder = pathStr.substring(0, lastSlash);
    if (!SD.exists(folder)) SD.mkdir(folder);
  }
}


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

void HelperUtility::delayMs(unsigned long ms)
{
  const unsigned long us = ms * 1000;
  const unsigned long start = micros();
  while (micros() - start < us) {}
}

void HelperUtility::drawWrappedCenterString(M5Canvas &canvas, const std::string &text)
{
  std::vector<std::string> lines;
  std::string current;
  const int maxWidth = canvas.width();

  size_t i = 0;
  while (i < text.size()) {
    while (i < text.size() && isspace(static_cast<unsigned char>(text[i]))) ++i;
    if (i >= text.size()) break;

    size_t j = i;
    while (j < text.size() && !isspace(static_cast<unsigned char>(text[j]))) ++j;
    std::string word = text.substr(i, j - i);
    i = j;

    auto fits = [&](const std::string &s) {
      return canvas.textWidth(s.c_str()) <= maxWidth;
    };

    std::string candidate = current.empty() ? word : current + " " + word;
    if (fits(candidate)) {
      current = candidate;
      continue;
    }

    if (current.empty()) {
      // single word longer than maxWidth -> break by characters
      std::string part;
      for (char c : word) {
        std::string tmp = part + c;
        if (!tmp.empty() && !fits(tmp)) {
          if (!part.empty()) lines.push_back(part);
          part.clear();
          part.push_back(c);
        } else {
          part.swap(tmp);
        }
      }
      if (!part.empty()) current = std::move(part);
    } else {
      lines.push_back(current);
      current.clear();
      // try placing the word into current (may still be too long)
      if (fits(word)) {
        current = word;
      } else {
        std::string part;
        for (char c : word) {
          std::string tmp = part + c;
          if (!tmp.empty() && !fits(tmp)) {
            if (!part.empty()) lines.push_back(part);
            part.clear();
            part.push_back(c);
          } else {
            part.swap(tmp);
          }
        }
        if (!part.empty()) current = std::move(part);
      }
    }
  }

  if (!current.empty()) lines.push_back(current);

  const int fh = canvas.fontHeight();
  const int totalH = fh * static_cast<int>(lines.size());
  const int startY = canvas.height() / 2 - totalH / 2;

  for (size_t idx = 0; idx < lines.size(); ++idx) {
    const std::string &l = lines[idx];
    const int tw = canvas.textWidth(l.c_str());
    const int x = canvas.width() / 2 - tw / 2;
    canvas.drawString(l.c_str(), x, startY + static_cast<int>(idx) * fh);
  }
}