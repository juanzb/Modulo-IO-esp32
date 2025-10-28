#include <handleBitEeprom/handleBitEeprom.hpp>
#include <EEPROM.h>

uint8_t handleBitEEprom::readBit(uint8_t &addr, uint8_t bit) {
  uint8_t reg = EEPROM.read(addr);
  return (reg >> bit) & 0x01;
}

void handleBitEEprom::modifyBit(uint8_t &addr, uint8_t bit, uint8_t &value) {  
  uint8_t reg = EEPROM.read(addr);
  reg &= ~(1 << bit);
  reg |= (value << bit);
  EEPROM.write(addr, reg);
  EEPROM.commit();
}

uint8_t handleBitEEprom::readHighNibble(uint8_t addr) {
  uint8_t value = EEPROM.read(addr);
  return (value >> 4) & 0x0F;
}

uint8_t handleBitEEprom::readLowNibble(uint8_t addr) {
  uint8_t value = EEPROM.read(addr);
  return value & 0x0F;
}

void handleBitEEprom::writeHighNibble(uint8_t addr, uint8_t newHigh) {
  uint8_t value = EEPROM.read(addr);
  newHigh &= 0x0F;         // Asegura 4 bits
  value &= 0x0F;           // Limpia nibble alto actual
  value |= (newHigh << 4); // Inserta nuevo nibble alto
  EEPROM.write(addr, value);
  EEPROM.commit();
}

void handleBitEEprom::writeLowNibble(uint8_t addr, uint8_t newLow) {
  uint8_t value = EEPROM.read(addr);
  newLow &= 0x0F;
  value &= 0xF0;           // Limpia nibble bajo actual
  value |= newLow;         // Inserta nuevo nibble bajo
  EEPROM.write(addr, value);
  EEPROM.commit();
}
