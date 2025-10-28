#include <handleInput.hpp>
#include <config_io.hpp>
#include <struct_io.hpp>

void startUpConfigIO() {
  static StrcInput in_1{GPIO_NUM_22, EEPROM_ADDR_B00, EEPROM_ADDR_B01};
  static StrcInput in_2{GPIO_NUM_23, EEPROM_ADDR_B02, EEPROM_ADDR_B03};
  static StrcOutput out_1{GPIO_NUM_5, EEPROM_ADDR_B04};
  static StrcOutput out_2{GPIO_NUM_18, EEPROM_ADDR_B05};
  static StrcOutput out_3{GPIO_NUM_2, EEPROM_ADDR_B06};

  static HandleOutput o1(out_1);
  static HandleOutput o2(out_2);
  static HandleOutput o3(out_3);
  static HandleInput i1(in_1);
  static HandleInput i2(in_2);

  HandleOutput::startUpOutputs();
  HandleInput::startUpInputs();

  Serial.print("INPUTS LOAD: ");
  Serial.println(HandleInput::getInstances().size());
  Serial.print("OUTPUTS LOAD: ");
  Serial.println(HandleOutput::getInstances().size());
}