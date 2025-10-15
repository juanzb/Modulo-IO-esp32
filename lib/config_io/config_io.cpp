#include <handleInput.hpp>
#include <config_io.hpp>
#include <struct_io.hpp>

StrcInput in_1{GPIO_NUM_22, EEPROM_ADDR_B00};
StrcInput in_2{GPIO_NUM_23, EEPROM_ADDR_B01};
StrcOutput out_1{GPIO_NUM_5, EEPROM_ADDR_B02};
StrcOutput out_2{GPIO_NUM_18, EEPROM_ADDR_B03};

HandleInput i1(in_1);
HandleInput i2(in_2);

HandleOutput o1(out_1);
HandleOutput o2(out_2);
