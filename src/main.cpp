#include <config_io.hpp>
#include <EEPROM.h>
#include <serverHttp.hpp>
#include <ApiServerHttp.hpp>
#include <connect_wifi.hpp>
#include <handleOutput.hpp>
#include <handleInput.hpp>


#define EEPROM_SIZE 12 // Tamaño de la EEPROM definida desde laEEPROM_ADDR_B00 hasta las EEPROM_ADDR_B11
WiFiManager wifi = WiFiManager::instance();

void setup() {
  Serial.begin(115200);
  EEPROM.begin(EEPROM_SIZE);
  wifi.begin();
  setupHttpServer();
  startUpConfigIO();
  }

void loop() {
  wifi.loop();
  HandleInput::loopInputs();
}

