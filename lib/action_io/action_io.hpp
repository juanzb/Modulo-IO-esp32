#pragma once
#include <Arduino.h>
#include <action_enum_state.hpp>
#include <cstdint>
#include <struct_io.hpp>
#include <time_press.hpp>
#include <vector>

class HandleInput {
  private:
    StrcInput inputIO;
    StrcOutput outputIOShortPress = StrcOutput(0, 0);
    StrcOutput outputIOLongPress = StrcOutput(0, 0);
    StateIO stateInput;
    uint8_t initStateOutputIOShortPress;
    uint8_t initStateOutputIOLongPress;
    uint64_t timeInputActive;
    uint64_t timeOfEvent;
    EventPress eventPressed;
    EventState stateEvent;
    
  public:
    static std::vector<HandleInput*> instances;  // lista global de instancias
    HandleInput(StrcInput _inputIO);

    static void setupInputs();
    static void loopInputs();   
    
  private:
    void configInitIO();
    void eventInputDetected();
    void actionByEventIdentified();
    void handleInputIO(EventPress &event);
    StateIO readInput(uint8_t &pinToRead);
    
  public:
    void setOutputIOShortPress(StrcOutput newPinOutput);
    void setOutputIOLongPress(StrcOutput newPinOutput);
    void setModeInputShortPress(ModeInput mode);
    void setModeInputLongPress(ModeInput mode);
};
