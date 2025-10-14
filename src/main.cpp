#include <action_io.hpp>
#include <config_io.hpp>
#include <EEPROM.h>
#include <serverHttp.hpp>
#include <connect_wifi.hpp>
#include <handleOutput.hpp>


#define EEPROM_SIZE 12 // Tamaño de la EEPROM definida desde laEEPROM_ADDR_B00 hasta las EEPROM_ADDR_B11
int stage = 0;
uint16_t timeStart = 0;

void setup() {
  Serial.begin(9600);
  EEPROM.begin(EEPROM_SIZE);
  wifiSetup();  
  setupHttpServer();
  HandleInput::setupInputs();
  HandleOutput::startUpOutputs();
  }

void loop() {
  
  HandleInput::loopInputs();

  if(stage == 0) {
    stage = 1;
    Serial.println(HandleInput::instances.size());
    HandleInput::instances.at(0)->setOutputIOShortPress(out_2);
    HandleInput::instances.at(1)->setOutputIOShortPress(out_1);
    HandleInput::instances.at(0)->setOutputIOLongPress(out_1);
    HandleInput::instances.at(1)->setOutputIOLongPress(out_2);
    Serial.println("Start ");
    timeStart = millis();
  }

  if (millis() - timeStart > 10000 && stage == 1) {
    stage = 2;
    HandleInput::instances.at(0)->setOutputIOShortPress(out_1);
    HandleInput::instances.at(1)->setOutputIOShortPress(out_2);
    Serial.println("Change IO");
  }
}

