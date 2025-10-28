#include <Arduino.h>
#include <cstdint>
#include <handleOutput.hpp>
#include <handleBitEeprom/handleBitEeprom.hpp>
#include <vector>

std::vector<HandleOutput*>& HandleOutput::getInstances() {
  static std::vector<HandleOutput*> instances;
  return instances;
}

HandleOutput::HandleOutput(StrcOutput _outputsList)
: output(_outputsList) {
  outputID = getInstances().size();
  getInstances().push_back(this);
}

void HandleOutput::startUpOutputs() {
  for (HandleOutput* out : HandleOutput::getInstances()) {
    pinMode(out->output.pin, OUTPUT);
    uint8_t registerGet = out->output.addrEeprom;
    uint8_t stateStartUpLastState = handleBitEEprom::readBit(registerGet, out->output.ADDR_BIT_ENABLE_STARTUP_LS);
    uint8_t valueStartUpLastState = handleBitEEprom::readBit(registerGet, out->output.ADDR_BIT_LAST_STATE);
    uint8_t valueStartUpDefault = handleBitEEprom::readBit(registerGet, out->output.ADDR_BIT_VALUE_STARTUP);
    uint8_t valueStart;
    if(stateStartUpLastState == 1) {
      valueStart = valueStartUpLastState;
    } else {
      valueStart = valueStartUpDefault;
    }
    Serial.print(String("OUTPUT ") + out->outputID + ", START UP, " );
    writeOutput(out->output, valueStart);
  }
  Serial.println("START UP OUTPUTS");
}

void HandleOutput::writeOutput(StrcOutput &output, uint8_t value) {
  Serial.println(String("OUTPUT ") + " WRITE" + " VALUE " + value); 
  digitalWrite(output.pin, value);
  updateSateOutput(output, value);
}

void HandleOutput::enableStartUpLastValue(StrcOutput &output, uint8_t value) {
  handleBitEEprom::modifyBit(output.addrEeprom, output.ADDR_BIT_ENABLE_STARTUP_LS, value);
}

void HandleOutput::setValueStartUp(StrcOutput &output, uint8_t value) {
  Serial.print("SET EEPROM VALUE START UP OUTPUT PIN: ");
  Serial.print(output.pin);
  Serial.print(", VALUE: ");
  Serial.println(value);
  handleBitEEprom::modifyBit(output.addrEeprom, output.ADDR_BIT_VALUE_STARTUP, value);
}

uint8_t HandleOutput::getSateOutput(StrcOutput &output) {
    Serial.print("GET STATE OUTPUT PIN: ");
    Serial.println(output.pin);
    uint8_t stateOutput = handleBitEEprom::readBit(output.addrEeprom, output.ADDR_BIT_LAST_STATE);
    return stateOutput;
}

void HandleOutput::updateSateOutput(StrcOutput &output, uint8_t value) {
  handleBitEEprom::modifyBit(output.addrEeprom, output.ADDR_BIT_LAST_STATE, value);
}

StrcOutput HandleOutput::getOutput() {
  return output;
}
