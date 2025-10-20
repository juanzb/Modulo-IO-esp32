#include <ApiServerHttp.hpp>
#include <handleOutput.hpp>
#include <time_press.hpp>

ApiServerHttp::ApiServerHttp() {}

// -------------- Outputs ------------------
void ApiServerHttp::getAmountOutputs(AsyncWebServerRequest *request) {
  request->send(200, "application/json", "{\"amountOutputs\":" + String(HandleOutput::getInstances().size()) + "}"); 
};


void ApiServerHttp::writeValueOutput(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (!docBody.containsKey("relay") || !docBody.containsKey("value")) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'relay' o 'value'\"}");
    return;
  }

  uint8_t relay = docBody["relay"];
  uint8_t value = docBody["value"];
  uint8_t outputID = relay-1;

  if (relay == 0 || relay > HandleOutput::getInstances().size()) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  }
  
  StrcOutput output = HandleOutput::getInstances().at(outputID)->getOutput();
  HandleOutput::writeOutput(output, value);
  const uint8_t stateOutput = HandleOutput::getSateOutput(output);

  // Responder en JSON
  JsonDocument response = DynamicJsonDocument (128);
  response["relay"] = relay;
  response["status"] = stateOutput;

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::setValueStartUp(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (!docBody.containsKey("relay") || !docBody.containsKey("value")) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'relay' o 'value'\"}");
    return;
  }

  uint8_t relay = docBody["relay"];
  uint8_t value = docBody["value"];
  uint8_t outputID = relay-1;

  if (relay == 0 || relay > HandleOutput::getInstances().size()) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  }
  
  StrcOutput output = HandleOutput::getInstances().at(outputID)->getOutput();
  HandleOutput::setValueStartUp(output, value);

  // Responder en JSON
  JsonDocument response = DynamicJsonDocument (128);
  response["relay"] = relay;
  response["stateStartUpValue"] = value;

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::enableStartUpLastState(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (!docBody.containsKey("relay") || !docBody.containsKey("value")) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'relay' o 'value'\"}");
    return;
  }

  uint8_t relay = docBody["relay"];
  uint8_t value = docBody["value"];
  uint8_t outputID = relay-1;

  if (relay == 0 || relay > HandleOutput::getInstances().size()) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  }
  
  StrcOutput output = HandleOutput::getInstances().at(outputID)->getOutput();
  HandleOutput::enableStartUpLastValue(output, value);

  // Responder en JSON
  JsonDocument response = DynamicJsonDocument (128);
  response["relay"] = relay;
  response["stateStartUpLastValue"] = value;

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
void ApiServerHttp::setTimeLongPress(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (!docBody.containsKey("timeLongPress")) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'timeLongPress'\"}");
    return;
  }

  uint8_t timeLongPress = docBody["timeLongPress"];

  if (timeLongPress == 0 ) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  };
  // const timeLP = TimePress
  // setTimeLongPress(timeLongPress);
  request->send(200, "application/json", "{\"status\":\"OK\"}");
};
