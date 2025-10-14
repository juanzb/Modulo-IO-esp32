#include <action_io.hpp>
#include <EEPROM.h>
#include <Arduino.h>
#include <time_press.hpp>
#include <handleBit.hpp>
#include <handleOutput.hpp>

  std::vector<HandleInput*> HandleInput::instances;    

  HandleInput::HandleInput(StrcInput _inputIO)
  : inputIO (_inputIO)
  {
    instances.push_back(this); 
  };


  void HandleInput::setupInputs() {
    for (HandleInput* io : HandleInput::instances) {
      io->configInitIO();
    }
  }


  void HandleInput::loopInputs() {
    for (HandleInput* io : HandleInput::instances) {
      io->eventInputDetected();
      io->actionByEventIdentified();
    }
  }


  void HandleInput::configInitIO() {
    pinMode(inputIO.pin, INPUT);
    stateInput = StateIO::INACTIVE;
    timeInputActive = 0;
    timeOfEvent = 0;
    eventPressed = EventPress::STANDBY;
    stateEvent = EventState::OUT_EVENT;
  }


  void HandleInput::eventInputDetected() {
    stateInput = readInput(inputIO.pin);
    if(stateEvent == EventState::OUT_EVENT && stateInput == StateIO::ACTIVE) {
      timeOfEvent = millis();
      stateEvent = EventState::IN_EVENT;
    }
  }

  
  StateIO HandleInput::readInput(uint8_t &pinToRead) {
    return digitalRead(pinToRead) == HIGH ? StateIO::ACTIVE : StateIO::INACTIVE;
  }


  void HandleInput::actionByEventIdentified() {
    if (stateEvent != EventState::IN_EVENT) return;
    timeInputActive = (millis() - timeOfEvent);
 
    if (stateInput == StateIO::ACTIVE) {
      if (timeInputActive >= timePress.LONG_PRESS_TIME && eventPressed != EventPress::LONG) {
        Serial.print("LONG PRESS, ");
        eventPressed = EventPress::LONG;
        HandleInput::handleInputIO(eventPressed);
      } 
    } else if (stateInput == StateIO::INACTIVE) {
      if (timeInputActive >= timePress.SHORT_PRESS_TIME  && timeInputActive < timePress.LONG_PRESS_TIME){
        Serial.print("SHORT PRESS, ");
        eventPressed = EventPress::SHORT;
        HandleInput::handleInputIO(eventPressed);
      }
      eventPressed = EventPress::STANDBY;
      stateEvent = EventState::OUT_EVENT;
    }
  }


  void HandleInput::handleInputIO(EventPress &event) {
    uint8_t registerEEPROM = EEPROM.read(inputIO.addrEeprom);
    uint8_t addrBitModeFuntion;
    uint8_t addrBitValueWrite;
    StrcOutput outputToWrite = StrcOutput(0);

    if (event == EventPress::SHORT) {
      addrBitModeFuntion = inputIO.ADDR_BIT_MODE_SP;
      addrBitValueWrite = inputIO.ADDR_BIT_VALUE_WRITE_SP;
      outputToWrite = outputIOShortPress;
    } else if ( event == EventPress::LONG) {
      addrBitModeFuntion = inputIO.ADDR_BIT_MODE_LP;
      addrBitValueWrite = inputIO.ADDR_BIT_VALUE_WRITE_LP;
      outputToWrite = outputIOLongPress;
    }

    uint8_t stateBitMode = handleBit::readBit(registerEEPROM, addrBitModeFuntion);
    ModeInput currentMode = stateBitMode ? ModeInput::COMMUTED : ModeInput::NORMAL;

    if(currentMode == ModeInput::NORMAL) {
      Serial.print("MODE NORMAL, ");
      uint8_t valueToSet = handleBit::readBit(registerEEPROM, addrBitValueWrite);
      HandleOutput::writeOutput(outputToWrite, valueToSet);
    } else if (currentMode == ModeInput::COMMUTED) {
      Serial.print("MODE COMMUTED, ");
      uint8_t registerEeprom = EEPROM.read(outputToWrite.addrEeprom);
      uint8_t currentStateOutput = handleBit::readBit(registerEeprom, outputToWrite.ADDR_BIT_LAST_STATE);
      uint8_t valueToSet = (currentStateOutput == 1) ? 0 : 1;
      HandleOutput::writeOutput(outputToWrite, valueToSet);
    }
  }


  void HandleInput::setOutputIOShortPress(StrcOutput newPinOutput) {
    Serial.print("SET OUTPUT SHORT PRESS: ");
    Serial.println(newPinOutput.pin);
    outputIOShortPress.pin = newPinOutput.pin;
    outputIOShortPress.addrEeprom = newPinOutput.addrEeprom;
    pinMode(newPinOutput.pin, OUTPUT);
  }


  void HandleInput::setOutputIOLongPress(StrcOutput newPinOutput) {
    Serial.print("SET OUTPUT LONG PRESS: ");
    Serial.println(newPinOutput.pin);
    outputIOLongPress.pin = newPinOutput.pin;
    outputIOLongPress.addrEeprom = newPinOutput.addrEeprom;
    pinMode(newPinOutput.pin, OUTPUT);
  }


  void HandleInput::setModeInputShortPress (ModeInput mode) {
    uint8_t registerEEPROM = EEPROM.read(inputIO.addrEeprom);
    uint8_t valueToSet = registerEEPROM;
    if (mode == ModeInput::NORMAL) {
      Serial.println("CHANGE INPUT SHORT PRESS AND NORMAL");
      valueToSet &= ~(1 << inputIO.ADDR_BIT_MODE_SP);
    } else if (mode == ModeInput::COMMUTED) {
      Serial.println("CHANGE INPUT SHORT PRESS AND COMMUTED");
      valueToSet |= (1 << inputIO.ADDR_BIT_MODE_SP);
    }
    EEPROM.write(inputIO.addrEeprom, valueToSet);
  }
  

    void HandleInput::setModeInputLongPress (ModeInput mode) {
    uint8_t registerEEPROM = EEPROM.read(inputIO.addrEeprom);
    uint8_t valueToSet = registerEEPROM;
    if (mode == ModeInput::NORMAL) {
      Serial.println("CHANGE INPUT LONG PRESS AND NORMAL");
      valueToSet &= ~(1 << inputIO.ADDR_BIT_MODE_LP);
    } else if (mode == ModeInput::COMMUTED) {
      Serial.println("CHANGE INPUT LONG PRESS AND COMMUTED");
      valueToSet |= (1 << inputIO.ADDR_BIT_MODE_LP);
    }
    EEPROM.write(inputIO.addrEeprom, valueToSet);
  }