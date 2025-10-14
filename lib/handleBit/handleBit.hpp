#include <stdint.h>

namespace handleBit {
  // registro, bit a leer 
  uint8_t readBit(uint8_t &reg, uint8_t bit);
  
  // registro, bit a modificar, valor
  uint8_t modifyBit(uint8_t &reg, uint8_t bit, uint8_t &value);
}