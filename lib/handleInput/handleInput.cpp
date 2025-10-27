#include <handleInput.hpp>
#include <EEPROM.h>
#include <Arduino.h>
#include <time_press.hpp>
#include <handleBit.hpp>
#include <handleOutput.hpp>

  HandleInput::HandleInput(StrcInput _input)
  : input (_input)
  {
    getInstances().push_back(this);
  };

  std::vector<HandleInput*>& HandleInput::getInstances() {
    static std::vector<HandleInput*> instances;
    return instances;
  }   

  void HandleInput::setupInputs() {
    for (HandleInput* io : HandleInput::getInstances()) {
      io->configInitInput();
    }
  }


  void HandleInput::loopInputs() {
    for (HandleInput* io : HandleInput::getInstances()) {
      io->eventInputDetected();
      io->actionByEventIdentified();
    }
  }


  void HandleInput::configInitInput() {
    pinMode(input.pin, INPUT);
    stateInput = StateIO::INACTIVE;
    timeInputActive = 0;
    timeOfEvent = 0;
    eventPressed = EventPress::STANDBY;
    stateEvent = EventState::OUT_EVENT;
    uint8_t outputToControlSP = handleBitEEprom::readLowNibble(input.addrEepromAux);
    uint8_t outputToControlLP = handleBitEEprom::readHighNibble(input.addrEepromAux);
    HandleInput::setOutputInputShortPress(outputToControlSP);
    HandleInput::setOutputInputLongPress(outputToControlLP);
  }


  void HandleInput::eventInputDetected() {
    stateInput = readInput(input.pin);
    if(stateEvent == EventState::OUT_EVENT && stateInput == StateIO::ACTIVE) {
      timeOfEvent = millis();
      stateEvent = EventState::IN_EVENT;
    }
  }

  
  StateIO HandleInput::readInput(uint8_t &pinToRead) {
    return digitalRead(pinToRead) == HIGH ? StateIO::ACTIVE : StateIO::INACTIVE;
  }


  void HandleInput::actionByEventIdentified() {
    EnableDisable stateEnableInputSP;
    EnableDisable stateEnableInputLP;
    
    if (stateEvent != EventState::IN_EVENT) return;
    timeInputActive = (millis() - timeOfEvent);
 
    if (stateInput == StateIO::ACTIVE) {
      if (timeInputActive >= timePress.LONG_PRESS_TIME && eventPressed != EventPress::LONG) {
        Serial.print("LONG PRESS, ");
        eventPressed = EventPress::LONG;
        stateEnableInputLP = static_cast<EnableDisable>(handleBitEEprom::readBit(input.addrEeprom, input.ADDR_BIT_ENABLE_LP));
        if (stateEnableInputLP == EnableDisable::ENABLE) {
          HandleInput::handleInputIO(eventPressed);
        } else {
          Serial.println("DISABLE");
        }
      }
    } else if (stateInput == StateIO::INACTIVE) {
      if (timeInputActive >= timePress.SHORT_PRESS_TIME  && timeInputActive < timePress.LONG_PRESS_TIME){
        stateEnableInputSP = static_cast<EnableDisable>(handleBitEEprom::readBit(input.addrEeprom, input.ADDR_BIT_ENABLE_SP));
        Serial.print("SHORT PRESS, ");
        eventPressed = EventPress::SHORT;
        if (stateEnableInputSP == EnableDisable::ENABLE) {
          HandleInput::handleInputIO(eventPressed);
        } else {
          Serial.println("DISABLE");
        }
      }
      eventPressed = EventPress::STANDBY;
      stateEvent = EventState::OUT_EVENT;
    }
  }


  void HandleInput::handleInputIO(EventPress &event) {
    uint8_t addrBitModeFuntion;
    uint8_t addrBitValueWrite;
    uint8_t outputToControl;
    StrcOutput outputToWrite = StrcOutput(0);

    if (event == EventPress::SHORT) {
      addrBitModeFuntion = input.ADDR_BIT_MODE_SP;
      addrBitValueWrite = input.ADDR_BIT_VALUE_WRITE_SP;
      outputToControl = handleBitEEprom::readLowNibble(input.addrEepromAux);
      outputToWrite = HandleOutput::getInstances()[outputToControl]->getOutput();
    } else if ( event == EventPress::LONG) {
      addrBitModeFuntion = input.ADDR_BIT_MODE_LP;
      addrBitValueWrite = input.ADDR_BIT_VALUE_WRITE_LP;
      outputToControl = handleBitEEprom::readHighNibble(input.addrEepromAux);
      outputToWrite = HandleOutput::getInstances()[outputToControl]->getOutput();
    }

    uint8_t stateBitMode = handleBitEEprom::readBit(input.addrEeprom, addrBitModeFuntion);
    ModeInput currentMode = static_cast<ModeInput>(stateBitMode);

    if(currentMode == ModeInput::NORMAL) {
      Serial.print("MODE NORMAL, ");
      uint8_t valueToSet = handleBitEEprom::readBit(input.addrEeprom, addrBitValueWrite);
      HandleOutput::writeOutput(outputToWrite, valueToSet);
    } else if (currentMode == ModeInput::COMMUTED) {
      Serial.print("MODE COMMUTED, ");
      uint8_t currentStateOutput = handleBitEEprom::readBit(outputToWrite.addrEeprom, outputToWrite.ADDR_BIT_LAST_STATE);
      uint8_t valueToSet = (currentStateOutput == 1) ? 0 : 1;
      HandleOutput::writeOutput(outputToWrite, valueToSet);
    }
  }


  void HandleInput::enableInputShortPress(EnableDisable value) {
    if (value == EnableDisable::DISABLE) {
      Serial.println("DISABLE INPUT SHORT PRESS");
    } else if (value == EnableDisable::ENABLE) {
      Serial.println("ENABLE INPUT SHORT PRESS");
    }
    uint8_t setValue = static_cast<uint8_t>(value);
    handleBitEEprom::modifyBit(input.addrEeprom, input.ADDR_BIT_ENABLE_SP, setValue);
  };


  void HandleInput::enableInputLongPress(EnableDisable value) {
    if (value == EnableDisable::DISABLE) {
      Serial.println("DISABLE INPUT LONG PRESS");
    } else if (value == EnableDisable::ENABLE) {
      Serial.println("ENABLE INPUT LONG PRESS");
    }
    uint8_t setValue = static_cast<uint8_t>(value);
    handleBitEEprom::modifyBit(input.addrEeprom, input.ADDR_BIT_ENABLE_LP, setValue);
  };


  void HandleInput::setOutputInputShortPress(uint8_t outputID) {
    Serial.print("SET OUTPUT SHORT PRESS: ");
    Serial.println(outputID);
    handleBitEEprom::writeLowNibble(input.addrEepromAux, outputID);
  }


  void HandleInput::setOutputInputLongPress(uint8_t outputID) {
    Serial.print("SET OUTPUT LONG PRESS: ");
    Serial.println(outputID);
    handleBitEEprom::writeHighNibble(input.addrEepromAux, outputID);
  }


  void HandleInput::setModeInputShortPress (ModeInput mode) {
    if (mode == ModeInput::NORMAL) {
      Serial.println("CHANGE INPUT SHORT PRESS TO NORMAL");
    } else if (mode == ModeInput::COMMUTED) {
      Serial.println("CHANGE INPUT SHORT PRESS TO COMMUTED");
    }
    uint8_t valueToSet = static_cast<uint8_t>(mode);
    handleBitEEprom::modifyBit(input.addrEeprom, input.ADDR_BIT_MODE_SP, valueToSet);
  }
  

  void HandleInput::setModeInputLongPress (ModeInput mode) {
    if (mode == ModeInput::NORMAL) {
      Serial.println("CHANGE INPUT LONG PRESS TO NORMAL");
    } else if (mode == ModeInput::COMMUTED) {
      Serial.println("CHANGE INPUT LONG PRESS TO COMMUTED");
    }
    uint8_t valueToSet = static_cast<uint8_t>(mode); 
    handleBitEEprom::modifyBit(input.addrEeprom, input.ADDR_BIT_MODE_LP, valueToSet);
  }
  
  void HandleInput::setValueNormalModeSP(uint8_t value) {
    Serial.print("SET VALUE INPUT SHORT PRESS: ");
    Serial.println(value);
    handleBitEEprom::modifyBit(input.addrEeprom, input.ADDR_BIT_VALUE_WRITE_SP, value);
  };
  
  
  void HandleInput::setValueNormalModeLP(uint8_t value) {
    Serial.print("SET VALUE INPUT LONG PRESS: ");
    Serial.println(value);
    handleBitEEprom::modifyBit(input.addrEeprom, input.ADDR_BIT_VALUE_WRITE_LP, value);
  };
  
  StrcInput HandleInput::getInput() {
    return input;
  }