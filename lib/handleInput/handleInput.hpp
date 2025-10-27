#pragma once
#include <Arduino.h>
#include <action_enum_state.hpp>
#include <cstdint>
#include <struct_io.hpp>
#include <time_press.hpp>
#include <vector>

class HandleInput {
  private:
    StrcInput input;
    StrcOutput outputToInputShortPress = StrcOutput(0, 0);
    StrcOutput outputToInputLongPress = StrcOutput(0, 0);
    StateIO stateInput;
    uint64_t timeInputActive;
    uint64_t timeOfEvent;
    EventPress eventPressed;
    EventState stateEvent;
    
  public:
    static std::vector<HandleInput*>& getInstances();

    HandleInput(StrcInput _input);

    static void setupInputs();
    static void loopInputs();
    StrcInput getInput();

    
  private:
    void configInitInput();
    void eventInputDetected();
    void actionByEventIdentified();
    void handleInputIO(EventPress &event);
    StateIO readInput(uint8_t &pinToRead);
    
  public:
    void enableInputShortPress(EnableDisable value);
    void enableInputLongPress(EnableDisable value);
    void setOutputInputShortPress(uint8_t outputID);
    void setOutputInputLongPress(uint8_t outputID);
    void setModeInputShortPress(ModeInput mode);
    void setModeInputLongPress(ModeInput mode);
    void setValueNormalModeSP(uint8_t value);
    void setValueNormalModeLP(uint8_t value);
};
