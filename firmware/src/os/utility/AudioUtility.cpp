//
// Created by l-sha on 30/10/2025.
//
#include "os/utility/AudioUtility.h"
#include "sounds/SoundNotification.h"
#include "sounds/SoundWin.h"
#include "sounds/SoundLose.h"

std::vector<int> AudioUtility::c_major_scale = {60, 62, 64, 65, 67, 69, 71};

void AudioUtility::playRandomTone(const int semitoneShift, const double durationSec)
{
  const auto speaker = &M5Cardputer.Speaker;
  if (speaker->getVolume() <= 0) return;

  const int randomIndex = random(0, c_major_scale.size());
  const int midi = c_major_scale[randomIndex] + semitoneShift;

  const double freq = 440.0 * std::pow(2.0, (midi - 69) / 12.0);
  if (speaker->isPlaying()) speaker->stop();
  speaker->tone(static_cast<int>(freq), durationSec * 1000);
}

void AudioUtility::playNotification()
{
  M5Cardputer.Speaker.playWav(NOTIFICATION_SOUND);
}

void AudioUtility::playLose()
{
  M5Cardputer.Speaker.playWav(LOSE_SOUND);
}

void AudioUtility::playWin()
{
  M5Cardputer.Speaker.playWav(WIN_SOUND);
}

void AudioUtility::playCorrectAnswer()
{
  if (M5Cardputer.Speaker.getVolume() <= 0) return;

  const int notes[] = {72, 79};
  for (int midi : notes) {
    const double freq = 440.0 * pow(2.0, (static_cast<double>(midi - 69) / 12.0));
    if (M5Cardputer.Speaker.isPlaying()) M5Cardputer.Speaker.stop();
    uint32_t duration = (midi == 72) ? 180 : 120;
    M5Cardputer.Speaker.tone(static_cast<uint16_t>(freq), duration);
    delay(100);
  }
}

void AudioUtility::playWrongAnswer()
{
  if (M5Cardputer.Speaker.getVolume() <= 0) return;

  const int notes[] = {85, 85};
  for (int midi : notes) {
    const double freq = 440.0 * pow(2.0, (static_cast<double>(midi - 69) / 12.0));
    if (M5Cardputer.Speaker.isPlaying()) M5Cardputer.Speaker.stop();
    M5Cardputer.Speaker.tone(static_cast<uint16_t>(freq), 150);
    delay(100);
  }
}
