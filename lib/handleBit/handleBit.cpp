#include <handleBit.hpp>

uint8_t handleBit::readBit(uint8_t &reg, uint8_t bit) {
  return (reg >> bit) & 0x01;
}

uint8_t handleBit::modifyBit(uint8_t &reg, uint8_t bit, uint8_t &value) {
  reg &= ~(1 << bit);
  reg |= (value << bit);
  return reg;
}