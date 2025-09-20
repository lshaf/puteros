//
// Created by l-sha on 9/20/2025.
//
#include "os/component/BatteryIndicator.hpp"

void drawBatteryIndicator(M5Canvas* container, const int x, const int y, const int percent)
{
    // Draw battery outline
    constexpr int width = 24;
    constexpr int height = 13;
    constexpr int tipWidth = 2;
    constexpr int tipHeight = height / 2;
    auto canvas = M5Canvas(&M5Cardputer.Lcd);

    canvas.createSprite(width + tipWidth, height);
    canvas.drawRoundRect(0, 0, width, height, 2, TFT_WHITE);
    canvas.fillRect(width, (height - tipHeight) / 2, tipWidth, tipHeight, TFT_WHITE);

    // Draw green bars for each 10%
    constexpr int barWidth = 2;
    const int barCount = percent / 10;
    canvas.fillRect(2, 2, barWidth * barCount, height - 4, barCount <= 2 ? TFT_RED : TFT_GREEN);

    // Draw percent in the middle
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", percent);
    const int textHeight = canvas.fontHeight();
    canvas.setTextColor(TFT_BLUE);
    canvas.drawCenterString(buf, width / 2, 3);
    canvas.pushSprite(container, x, y);
}