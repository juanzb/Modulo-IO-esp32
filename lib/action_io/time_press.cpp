#include <time_press.hpp>
#include <stdint.h>

TimePress::TimePress(uint16_t shortPress, uint16_t longPress):
  SHORT_PRESS_TIME(shortPress),
  LONG_PRESS_TIME(longPress)
{};

void TimePress::setTimeShortPress(uint16_t shortPress) {
  SHORT_PRESS_TIME= shortPress;
};

void TimePress::setTimeLongPress(uint16_t longPress) {
  LONG_PRESS_TIME = longPress;
};

TimePress timePress(50, 1200);