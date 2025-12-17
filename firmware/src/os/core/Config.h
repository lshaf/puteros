//
// Created by l-sha on 9/23/2025.
//

#pragma once
#include <SD.h>
#include <Arduino.h>
#include <map>

#include "os/utility/HelperUtility.h"

class Config {
private:
  const char* config_path;
  std::map<String, String> config_;

public:
  explicit Config(const char* filename): config_path(filename)
  {
    load();
  }

  void set(const String& key, const String& value) {
    config_[key] = value;
  }

  String get(const String& key, const String& defaultValue = "") const {
    const auto it = config_.find(key);
    return it != config_.end() ? it->second : defaultValue;
  }

  bool has(const String& key) const {
    return config_.find(key) != config_.end();
  }

  bool load() {
    File file = SD.open(config_path, FILE_READ);
    if (!file) return false;
    config_.clear();
    while (file.available()) {
      String line = file.readStringUntil('\n');
      const int sep = line.indexOf('=');
      if (sep > 0) {
        const String key = line.substring(0, sep);
        String value = line.substring(sep + 1);
        value.trim();
        config_[key] = value;
      }
    }
    file.close();
    return true;
  }

  bool save() const {
    HelperUtility::makeDirectoryRecursive(config_path);
    File file = SD.open(config_path, FILE_WRITE);
    if (!file) return false;
    for (const auto& kv : config_) {
      file.print(kv.first);
      file.print("=");
      file.println(kv.second);
    }
    file.close();
    return true;
  }
};
