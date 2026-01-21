//
// Created by l-sha on 09/10/2025.
//

#include <SD.h>
#include <Arduino.h>
#include <Preferences.h>
#include "esp_system.h"
#include "os/utility/HelperUtility.h"

std::string HelperUtility::generateRandomString(size_t length)
{
  if (length == 0) return {};

  constexpr size_t MAX_LEN = 4096;
  if (length > MAX_LEN) length = MAX_LEN;

  static constexpr char charset[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
  static constexpr size_t charsetSize = sizeof(charset) - 1;

  std::string out;
  out.reserve(length);

  while (out.size() < length) {
    const uint32_t r = trueRandom(charsetSize);
    out.push_back(charset[r]);
  }

  return out;
}


void HelperUtility::makeDirectoryRecursive(const std::string& path)
{
  if (path.empty() || SD.exists(path.c_str())) return;

  const auto pathStr = String(path.c_str());
  const int lastSlash = pathStr.lastIndexOf('/');

  if (lastSlash > 0) {
    const String parent = pathStr.substring(0, lastSlash);
    makeDirectoryRecursive(parent.c_str()); // recursive call to create parent chain
  }

  SD.mkdir(path.c_str());
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

bool HelperUtility::parseInt32(const std::string& input, int32_t &value) {
  const char *str = input.c_str();
  while (*str == ' ' || *str == '\t' || *str == '\r' || *str == '\n') ++str;

  errno = 0;
  char *end = nullptr;
  long v = std::strtol(str, &end, 10);

  if (str == end) return false; // no digits
  if (errno == ERANGE || v < INT32_MIN || v > INT32_MAX) return false;

  while (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n') ++end;
  if (*end != '\0') return false; // trailing non-space characters

  value = static_cast<int32_t>(v);
  return true;
}

void HelperUtility::shuffleSeed()
{
  Preferences prefs;
  prefs.begin("game_decoder", false);
  const uint32_t last = prefs.getUInt("seed", 0);
  const uint32_t hw = esp_random();
  const auto t = static_cast<uint32_t>(millis());

  const uint32_t mix = t * 1664525u + 1013904223u;
  uint32_t newSeed = last ^ hw ^ mix;
  if (newSeed == 0) newSeed = hw ? hw : 0xA5A5A5A5u;

  prefs.putUInt("seed", newSeed);
  prefs.end();

  randomSeed(newSeed);
}

long HelperUtility::trueRandom(const long max) {
  shuffleSeed();
  return random(max);
}

String HelperUtility::padNumber(int number, int width)
{
  String str = String(number);
  while (str.length() < width)
  {
    str = "0" + str;
  }
  return str;
}