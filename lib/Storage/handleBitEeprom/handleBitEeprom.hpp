#include <stdint.h>

namespace handleBitEEprom {
  // registro, bit a leer 
  uint8_t readBit(uint8_t &addr, uint8_t bit);
  
  // registro, bit a modificar, valor
  void modifyBit(uint8_t &addr, uint8_t bit, uint8_t &value);

  // Función para obtener la mitad alto
  uint8_t readHighNibble(uint8_t value);

  // Función para obtener la mitad bajo
  uint8_t readLowNibble(uint8_t value);

  // Función para modificar la mitad alto (0–15)
  void writeHighNibble(uint8_t addr, uint8_t newHigh);

  // Función para modificar la mitad bajo (0–15)
  void writeLowNibble(uint8_t addr, uint8_t newLow);
}