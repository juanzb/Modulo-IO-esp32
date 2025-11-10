#include <ApiServerHttp.hpp>
#include <handleOutput.hpp>
#include <handleInput.hpp>
#include <connect_wifi.hpp>
#include <time_press.hpp>
#include <action_enum_state.hpp>

ApiServerHttp::ApiServerHttp() {}
// ApiServerHttp apiServerHandler;

// ===================== Outputs =====================
void ApiServerHttp::getAmountOutputs(AsyncWebServerRequest *request) {
  request->send(200, "application/json", "{\"amountOutputs\":" + String(HandleOutput::getInstances().size()) + "}"); 
};


void ApiServerHttp::writeValueOutput(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (docBody["output"].isNull() || docBody["value"].isNull()) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'output' o 'value'\"}");
    return;
  }

  uint8_t outputID = docBody["output"];
  uint8_t value = docBody["value"];

  if (outputID < 0 || outputID > HandleOutput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  }
  
  StrcOutput output = HandleOutput::getInstances().at(outputID)->getOutput();
  HandleOutput::writeOutput(output, value);
  const uint8_t stateOutput = HandleOutput::getSateOutput(output);

  // Responder en JSON
  JsonDocument response;
  response["output"] = outputID;
  response["status"] = stateOutput;

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::setValueStartUp(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (docBody["output"].isNull() || docBody["value"].isNull()) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'output' o 'value'\"}");
    return;
  }

  uint8_t outputID = docBody["output"];
  uint8_t value = docBody["value"];

  if (outputID < 0 || outputID > HandleOutput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  }
  
  StrcOutput output = HandleOutput::getInstances().at(outputID)->getOutput();
  HandleOutput::setValueStartUp(output, value);

  // Responder en JSON
  JsonDocument response;
  response["output"] = outputID;
  response["stateStartUpValue"] = value;

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::enableStartUpLastState(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (docBody["output"].isNull() || docBody["value"].isNull()) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'output' o 'value'\"}");
    return;
  }

  uint8_t outputID = docBody["output"];
  uint8_t value = docBody["value"];

  if (outputID < 0 || outputID > HandleOutput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice inválido\"}");
    return;
  }
  
  StrcOutput output = HandleOutput::getInstances().at(outputID)->getOutput();
  HandleOutput::enableStartUpLastValue(output, value);

  // Responder en JSON
  JsonDocument response;
  response["output"] = outputID;
  response["stateStartUpLastValue"] = value;

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};




// ===================== Inputs =====================
void ApiServerHttp::getAmountInputs(AsyncWebServerRequest *request) {
  request->send(200, "application/json", "{\"amountInputs\":" + String(HandleInput::getInstances().size()) + "}"); 
};


void ApiServerHttp::enableInput(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (
    docBody["input"].isNull() 
    || docBody["valueEnable"].isNull() 
    || docBody["pressType"].isNull()
  ) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'input', 'valueEnable' o 'pressType'\"}");
    return;
  }

  uint8_t inputID = docBody["input"];
  uint8_t valueEnableID = docBody["valueEnable"];
  uint8_t pressTypeID = docBody["pressType"];
  EnableDisable valueEnable = static_cast<EnableDisable>(valueEnableID);

  if (inputID < 0 || inputID > HandleInput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice input inválido\"}");
    return;
  } else if (valueEnableID < 0 || valueEnableID > 1) {
    request->send(400, "application/json", "{\"error\":\"Índice valueEnable inválido\"}");
    return;
  } else if (pressTypeID < 0 || pressTypeID > 1) {
    request->send(400, "application/json", "{\"error\":\"Índice pressType inválido\"}");
    return;
  }
  
  JsonDocument response;
  
  if (pressTypeID == 0) { // Short Press
    HandleInput::getInstances().at(inputID)->enableInputShortPress(valueEnable);
    response["input"] = inputID;
    response["statusSortPress"] = valueEnableID ? "enable" : "disable";
  } else { // Long Press
    HandleInput::getInstances().at(inputID)->enableInputLongPress(valueEnable);
    response["input"] = inputID;
    response["statusLongPress"] = valueEnableID ? "enable" : "disable";
  }

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::setModeInput(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (
    docBody["input"].isNull() 
    || docBody["mode"].isNull() 
    || docBody["pressType"].isNull()
  ) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'input', 'mode' o 'pressType'\"}");
    return;
  }

  uint8_t inputID = docBody["input"];
  uint8_t modeID = docBody["mode"];
  uint8_t pressTypeID = docBody["pressType"];
  ModeInput mode = static_cast<ModeInput>(modeID);

  if (inputID < 0 || inputID > HandleInput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice input inválido\"}");
    return;
  } else if (modeID < 0 || modeID > 1) {
    request->send(400, "application/json", "{\"error\":\"Índice mode inválido\"}");
    return;
  } else if (pressTypeID < 0 || pressTypeID > 1) {
    request->send(400, "application/json", "{\"error\":\"Índice pressType inválido\"}");
    return;
  }

  JsonDocument response;
  
  if (pressTypeID == 0) { // Short Press
    HandleInput::getInstances().at(inputID)->setModeInputShortPress(mode);
    response["input"] = inputID;
    response["modeShortPress"] = modeID;
  } else { // Long Press
    HandleInput::getInstances().at(inputID)->setModeInputLongPress(mode);
    response["input"] = inputID;
    response["modeLongPress"] = modeID;
  }

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::setOutputToInput(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (
    docBody["input"].isNull()
    || docBody["output"].isNull()
    || docBody["pressType"].isNull()
  ) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'input', 'output' o 'pressType'\"}");
    return;
  }

  uint8_t inputID = docBody["input"];
  uint8_t outputID = docBody["output"];
  uint8_t pressTypeID = docBody["pressType"];

  if (inputID < 0 || inputID > HandleInput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice input inválido\"}");
    return;
  } else if (outputID < 0 || outputID > HandleOutput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice output inválido\"}");
    return;
  } else if (pressTypeID < 0 || pressTypeID > 1) {
    request->send(400, "application/json", "{\"error\":\"Índice pressType inválido\"}");
    return;
  }
  
  JsonDocument response;
  
  if (pressTypeID == 0) { // Short Press
    HandleInput::getInstances().at(inputID)->setOutputInputShortPress(outputID);
    response["input"] = inputID;
    response["outputShortPress"] = outputID;
  } else { // Long Press
    HandleInput::getInstances().at(inputID)->setOutputInputLongPress(outputID);
    response["input"] = inputID;
    response["outputLongPress"] = outputID;
  }

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::setValueModeNormalInput(JsonDocument &docBody, AsyncWebServerRequest *request) {
    if (
    docBody["input"].isNull() 
    || docBody["value"].isNull() 
    || docBody["pressType"].isNull()
  ) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'input', 'value' o 'pressType'\"}");
    return;
  }

  uint8_t inputID = docBody["input"];
  uint8_t value = docBody["value"];
  uint8_t pressTypeID = docBody["pressType"];

  if (inputID < 0 || inputID > HandleInput::getInstances().size()-1) {
    request->send(400, "application/json", "{\"error\":\"Índice input inválido\"}");
    return;
  } else if (value < 0 || value > 1) {
    request->send(400, "application/json", "{\"error\":\"Índice value inválido\"}");
    return;
  } else if (pressTypeID < 0 || pressTypeID < 0) {
    request->send(400, "application/json", "{\"error\":\"Índice pressType inválido\"}");
    return;
  }
  
  JsonDocument response;
  
  if (pressTypeID == 0) { // Short Press
    HandleInput::getInstances().at(inputID)->setValueNormalModeSP(value);
    response["input"] = inputID;
    response["valueNormalModeSP"] = value;
  } else { // Long Press
    HandleInput::getInstances().at(inputID)->setValueNormalModeLP(value);
    response["input"] = inputID;
    response["valueNormalModeLP"] = value;
  }

  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};




// ===================== Wifi =====================
void ApiServerHttp::accesPointWifi(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (docBody["enableAP"].isNull()) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'enableAP'\"}");
    return;
  }
  
  WiFiManager& wifi = WiFiManager::instance();
  bool enableAP = docBody["enableAP"];
  JsonDocument response;
  response["enableAP"] = enableAP;
  
  if (enableAP) {
    if (wifi.startAccessPoint("12345678")) {
      response["description"] = "modo AP habilitado correctamente";
      response["ip"] = wifi.getLocalIP().toString();
    } else {
      response["description"] = "Error al habilitar el modo AP";
    }
  } else {
    response["description"] = "modo AP deshabilitado correctamente";
    wifi.stopAccessPoint();
  }
  
  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::conectToWifi(JsonDocument &docBody, AsyncWebServerRequest *request) {
  
  if (docBody["ssid"].isNull() || docBody["password"].isNull()) {
    request->send(400, "application/json", "{\"error\":\"Faltan campos 'ssid', 'password', 'apMode' o 'deviceID'\"}");
    return;
  }
  
  WiFiManager& wifi = WiFiManager::instance();
  String ssid = docBody["ssid"];
  String password = docBody["password"];

  JsonDocument response;
  response["ssid"] = ssid;
  
  bool stateSaveConfig = wifi.changeNetwork(ssid, password);
  if (stateSaveConfig) {
    response["description"] = "Conectado correctamente a la red";
    response["ip"] = wifi.getLocalIP().toString();
  } else {
    response["description"] = "No se pudo conectar, AP activado como fallback";
  }
  
  String outRequest;
  serializeJson(response, outRequest);
  request->send(200, "application/json", outRequest);
};


void ApiServerHttp::scannerWifi(AsyncWebServerRequest *request) {
  WiFiManager& wifi = WiFiManager::instance();
  JsonDocument response;

  if (wifi.getScanState() == ScanStatus::SCANNING) {
    response["description"] = "Ya hay un escaneo en curso";
  } else {
    wifi.startScanAsyncTask();
    response["description"] = "Escaneo iniciado correctamente";
  }

  String out;
  serializeJson(response, out);
  request->send(200, "application/json", out);
};


void ApiServerHttp::getScannerWifiResult(AsyncWebServerRequest *request) {
    WiFiManager& wifi = WiFiManager::instance();
    JsonDocument response;

    if (wifi.getScanState() == ScanStatus::SCANNING) {
        response["status"] = "scanning";
        response["description"] = "Escaneo aún en progreso";
    } else if (wifi.getScanState() == ScanStatus::SUCCESS) {
        response["status"] = "success";
        JsonArray networks = response.createNestedArray("networks");

        auto results = wifi.consumeScanResults();
        for (auto &net : results) {
            JsonObject obj = networks.createNestedObject();
            obj["ssid"] = net["ssid"];
            obj["rssi"] = net["rssi"];
            obj["channel"] = net["channel"];
            obj["encryption"] = net["encryption"];
        }
    } else {
        response["status"] = "no_scan";
        response["description"] = "No hay resultados de escaneo disponibles";
    }

    String out;
    serializeJson(response, out);
    request->send(200, "application/json", out);
};




// ===================== Time Press =====================
void ApiServerHttp::setTimeLongPress(JsonDocument &docBody, AsyncWebServerRequest *request) {
  if (docBody["timeLongPress"].isNull()) {
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
