#include <ApiServerHttp.hpp>
#include <handleOutput.hpp>

ApiServerHttp::ApiServerHttp() {}

// -------------- Outputs ------------------
void ApiServerHttp::getAmountOutputs(AsyncWebServerRequest *request) {};
void ApiServerHttp::enableStartUpLastState(JsonDocument &docBody, AsyncWebServerRequest *request) {};

void ApiServerHttp::setValueStartUpLastState(JsonDocument &docBody, AsyncWebServerRequest *request) {

  if (!docBody.containsKey("relay") || !docBody.containsKey("action")) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'relay' o 'action'\"}");
    return;
  }

  uint8_t relay = docBody["relay"];
  uint8_t action = docBody["action"];
  uint8_t outputID = relay-1;

  if (relay == 0 || relay > HandleOutput::instances.size()) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  }
  
  StrcOutput output = HandleOutput::instances.at(outputID)->getOutput();
  HandleOutput::writeOutput(output, action);
  const uint8_t stateOutput = HandleOutput::getSateOutput(output);

  // Responder en JSON
  JsonDocument response = DynamicJsonDocument (128);
  response["status"] = stateOutput;
  response["relay"] = relay;

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


// -------------- Inputs -------------------
// Short Press
void ApiServerHttp::enableInputSP(JsonDocument &docBody, AsyncWebServerRequest *request) {};
void ApiServerHttp::setModeInputSP(JsonDocument &docBody, AsyncWebServerRequest *request) {};
void ApiServerHttp::setOutputToInputSP(JsonDocument &docBody, AsyncWebServerRequest *request) {};
void ApiServerHttp::setValueModeNormalSP(JsonDocument &docBody, AsyncWebServerRequest *request) {};
// Long Press
void ApiServerHttp::enableInputLP(JsonDocument &docBody, AsyncWebServerRequest *request) {};
void ApiServerHttp::setModeInputLP(JsonDocument &docBody, AsyncWebServerRequest *request) {};
void ApiServerHttp::setOutputToInputLP(JsonDocument &docBody, AsyncWebServerRequest *request) {};
void ApiServerHttp::setValueModeNormalLP(JsonDocument &docBody, AsyncWebServerRequest *request) {};


// -------------- Time Press -------------------
void ApiServerHttp::setTimeShortPress(JsonDocument &docBody, AsyncWebServerRequest *request) {};