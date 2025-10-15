#include <Arduino.h>
#include <cstdint>
#include <EEPROM.h>
#include <handleBit.hpp>
#include <handleOutput.hpp>
#include <vector>


std::vector<HandleOutput*>& HandleOutput::getInstances() {
  static std::vector<HandleOutput*> instances;
  return instances;
}

HandleOutput::HandleOutput(StrcOutput _outputsList)
: output(_outputsList) {
  getInstances().push_back(this);
}

void HandleOutput::startUpOutputs() {
  for (HandleOutput* out : HandleOutput::getInstances()) {
    pinMode(out->output.pin, OUTPUT);
    uint8_t registerGet = EEPROM.read(out->output.addrEeprom);
    uint8_t valueStart = handleBit::readBit(registerGet, out->output.ADDR_BIT_LAST_STATE);
    digitalWrite(out->output.pin, valueStart);
  }
  Serial.println("START UP OUTPUTS");
}

void HandleOutput::writeOutput(StrcOutput &output, uint8_t value) {
  Serial.print("WRITE OUTPUT PIN: ");
  Serial.print(output.pin);
  Serial.print(", VALUE: ");
  Serial.println(value);
  digitalWrite(output.pin, value);
  updateSateOutput(output, value);
}

void HandleOutput::enableStartUpLastValue(StrcOutput &output, uint8_t value) {
  uint8_t registerEeprom = EEPROM.read(output.addrEeprom);
  registerEeprom &= ~(1 << output.ADDR_BIT_ENABLE_STARTUP_LS);
  registerEeprom |= (value << output.ADDR_BIT_ENABLE_STARTUP_LS);
  EEPROM.write(output.addrEeprom, registerEeprom);
  EEPROM.commit();
}

void HandleOutput::setValueStartUp(StrcOutput &output, uint8_t value) {
  uint8_t registerEeprom = EEPROM.read(output.addrEeprom);
  registerEeprom &= ~(1 << output.ADDR_BIT_VALUE_STARTUP);
  registerEeprom |= (value << output.ADDR_BIT_VALUE_STARTUP);
  EEPROM.write(output.addrEeprom, registerEeprom);
  EEPROM.commit();
}

uint8_t HandleOutput::getSateOutput(StrcOutput &output) {
    uint8_t registerGet = EEPROM.read(output.addrEeprom);
    uint8_t stateOutput = handleBit::readBit(registerGet, output.ADDR_BIT_LAST_STATE);
    return stateOutput;
}

void HandleOutput::updateSateOutput(StrcOutput &output, uint8_t value) {
  uint8_t registerEeprom = EEPROM.read(output.addrEeprom);
  registerEeprom &= ~(1 << output.ADDR_BIT_LAST_STATE);
  registerEeprom |= (value << output.ADDR_BIT_LAST_STATE);
  EEPROM.write(output.addrEeprom, registerEeprom);
  EEPROM.commit();
}

StrcOutput HandleOutput::getOutput() {
  return output;
}