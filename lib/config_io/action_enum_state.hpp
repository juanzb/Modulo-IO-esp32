#pragma once
#include <cstdint>

/**
 * Reset = 0, state initial of button (not pressed)
 * Short = 1, detection of pressed button for a short time
 * Long = 2, detection of pressed button for a long time
 */
enum class EventPress: uint8_t {
  STANDBY = 0,
  SHORT = 1,
  LONG = 2,
};

/**
 * Out Event = 0, button return to initial state (not pressed)
 * In Event = 1, button is pressed on the short or long time
 */
enum class EventState: uint8_t {
  OUT_EVENT = 0,
  IN_EVENT = 1,
};

/**
 * Inactive = 0, input/output is LOW
 * Active = 1, input/output is HIGH
 */
enum class StateIO: uint8_t {
  INACTIVE = 0,
  ACTIVE = 1,
};


/**
 * Inactive = 0, input/output is LOW
 * Active = 1, input/output is HIGH
 */
enum class EnableDisable: uint8_t {
  DISABLE = 0,
  ENABLE = 1,
};


/**
 * Normal = 0, Commutated = 1
 */
enum class ModeInput: uint8_t {
  NORMAL = 0,
  COMMUTED = 1,
};
