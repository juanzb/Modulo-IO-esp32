#pragma once
#include <cstdint>

class TimePress {
  public:
    uint16_t SHORT_PRESS_TIME;
    uint16_t LONG_PRESS_TIME;
  
  public:
    TimePress(uint16_t shortPress = 50, uint16_t longPress = 1200);
    void setTimeShortPress(uint16_t shortPress);
    void setTimeLongPress(uint16_t longPress);
};

extern TimePress timePress;