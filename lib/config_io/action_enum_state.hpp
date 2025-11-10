#pragma once
#include <cstdint>

/**
 * Eventos detectados al presionar un botón.
 * 
 * - stadby, state initial of button (not pressed)
 * - Short, detection of pressed button for a short time
 * - Long, detection of pressed button for a long time
 */
enum class EventPress: uint8_t {
  STANDBY, // state initial of button (not pressed)
  SHORT,   // detection of pressed button for a short time
  LONG     // detection of pressed button for a long time
};

/**
 * Estado del evento de botón.
 * 
 * - OutEvent, button return to initial state (not pressed)
 * - InEvent, button is pressed on the short or long time
 */
enum class EventState: uint8_t {
  OUT_EVENT, // button return to initial state (not pressed)
  IN_EVENT   // button is pressed on the short or long time
};

/**
 * Estado lógico de una entrada o salida digital.
 * 
 * - Inactive, input/output is LOW
 * - Active, input/output is HIGH
 */
enum class StateIO: uint8_t {
  INACTIVE, // input/output is LOW
  ACTIVE    // input/output is HIGH
};


/**
 * Estado de habilitación o deshabilitación de una función.
 * 
 * - Inactive, input/output is LOW
 * - Active, input/output is HIGH
 */
enum class EnableDisable: uint8_t {
  DISABLE, // Funcionalidad desactivada
  ENABLE   // Funcionalidad activada
};


/**
 * Modo de control de una entrada sobre la salida
 * 
 * - Normal 
 * - Commutated
 */
enum class ModeInput: uint8_t {
  NORMAL,   // Lectura directa (sin invertir)
  COMMUTED  // Lectura conmutada (invertida)
};
