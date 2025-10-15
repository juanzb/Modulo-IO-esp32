#pragma once
#include <cstdint>
#include <struct_io.hpp>
#include <action_enum_state.hpp>
#include <vector>

class HandleOutput {
  private:
    StrcOutput output;

  public:
    static std::vector<HandleOutput*>& getInstances();
    HandleOutput(StrcOutput _outputsList);
    static void startUpOutputs();
    static void writeOutput(StrcOutput &output, uint8_t value);
    static void enableStartUpLastValue(StrcOutput &output, uint8_t value);
    static uint8_t getSateOutput(StrcOutput &output);
    static void setValueStartUp(StrcOutput &output, uint8_t value );
    StrcOutput getOutput();

  private:
    static void updateSateOutput(StrcOutput &output, uint8_t value);
};
