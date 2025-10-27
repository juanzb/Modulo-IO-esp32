#include <config_io.hpp>
#include <EEPROM.h>
#include <serverHttp.hpp>
#include <connect_wifi.hpp>
#include <handleOutput.hpp>
#include <handleInput.hpp>


#define EEPROM_SIZE 12 // Tamaño de la EEPROM definida desde laEEPROM_ADDR_B00 hasta las EEPROM_ADDR_B11
int stage = 0;
uint16_t timeStart = 0;

void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  wifiSetup();
  setupHttpServer();
  startUpConfigIO();
  }

void loop() {
  HandleInput::loopInputs();
}

