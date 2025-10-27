#pragma once
#include <defineEEPRON.hpp>
#include <cstdint>

/**
 * @struct StrcOutput.
 * 
 * @param pin Número del pin físico del microcontrolador asignado a la salida.
 * @param addrEeprom Dirección en EEPROM donde se almacena la configuración de esta salida.
 */
struct StrcOutput {
  static const uint8_t ADDR_BIT_LAST_STATE = 0;        // Número del bit de último estado de la salida.
  static const uint8_t ADDR_BIT_ENABLE_STARTUP_LS = 1; // Número del bit para restaurar el último estado al iniciar.
  static const uint8_t ADDR_BIT_VALUE_STARTUP = 2;     // Número del bit para el valor de inicio de la salida.
  
  uint8_t pin;
  uint8_t addrEeprom;
  
  StrcOutput(uint8_t _pin, uint8_t _addrEeprom = 0)
  : pin(_pin), addrEeprom(_addrEeprom)
  {}
};



/**
 * @struct StrcInput.
 * 
 * @param pin Número del pin físico para input del microcontrolador.
 * @param addrEeprom Dirección en la EEPROM donde se almacena el byte de configuración de la entrada.
 * @param addrEeprom Dirección en la EEPROM como AUX donde se almacena el byte de configuración de la entrada del puslo largo o corto.
 */
struct StrcInput {
  static const uint8_t ADDR_BIT_ENABLE_SP = 0;       // Número del bit para habilitar o deshabilitar la entrada.
  static const uint8_t ADDR_BIT_MODE_SP = 1;         // Número del bit para modo de funcionamiento del Short Press.
  static const uint8_t ADDR_BIT_VALUE_WRITE_SP = 2;  // Número del bit para valor a escribir al accionar con Short Press.
  static const uint8_t ADDR_BIT_ENABLE_LP = 4;       // Número del bit para habilitar lógica de Long Press.
  static const uint8_t ADDR_BIT_MODE_LP = 5;         // Número del bit para modo de funcionamiento del Long Press.
  static const uint8_t ADDR_BIT_VALUE_WRITE_LP = 6;  // Número del bit para valor a escribir al accionar con Long Press.
  
  uint8_t pin; 
  uint8_t addrEeprom;
  uint8_t addrEepromAux; 

  StrcInput(uint8_t _pin, uint8_t _addrEeprom, uint8_t _addrEepromAux)
  : pin(_pin), addrEeprom(_addrEeprom), addrEepromAux(_addrEepromAux)
  {}
};
