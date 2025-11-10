#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <handleOutput.hpp>
#include <struct_io.hpp>
#include <apiServerHttp.hpp>

AsyncWebServer server(80);
ApiServerHttp apiServerHandler;

// ==================== Función para parsear JSON ====================
void parseBody(AsyncWebServerRequest *request, uint8_t *data, size_t len, String &bodyString, JsonDocument &bodyDoc) {
  bodyString.reserve(len);
  bodyString.concat((const char*)data, len);
  // Serial.println(bodyString);

  DeserializationError err = deserializeJson(bodyDoc, bodyString);
  if (err) {
    // Serial.print("Error deserializando JSON: ");
    // Serial.println(err.f_str());
    request->send(400, "application/json", "{\"error\":\"JSON inválido\"}");
    return;
  }
}


// ===================== setup =====================
void setupHttpServer() {
  
  // ====================== OUTPUTS ======================
  server.on("/api/output/amount", 
    HTTP_GET, 
    [](AsyncWebServerRequest *request) {
      ApiServerHttp::getAmountOutputs(request);
    }
  );

  server.on(
    "/api/output/action", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::writeValueOutput(bodyDoc, request);
    }
  );

  server.on(
    "/api/output/setup/startUp/value", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,   
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::setValueStartUp(bodyDoc, request);
    }
  );

  server.on(
    "/api/output/setup/startUp/lastState", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,   
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::enableStartUpLastState(bodyDoc, request);
    }
  );



  // ========================= INPUTS =========================
  server.on(
    "/api/input/amount", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::getAmountInputs(request);
    }
  );

  server.on(
    "/api/input/enable",
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::enableInput(bodyDoc, request);
    }
  );

  server.on(
    "/api/input/mode", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::setModeInput(bodyDoc, request);
    }
  );

  server.on(
    "/api/input/set/output", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::setOutputToInput(bodyDoc, request);
    }
  );

  server.on(
    "/api/input/setup/mode/normal/value", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::setValueModeNormalInput(bodyDoc, request);
    }
  );



  // ========================= Wifi =========================
  server.on(
    "/api/setup/wifi/AP", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      apiServerHandler.accesPointWifi(bodyDoc, request);
    }
  );

  server.on(
    "/api/setup/wifi/connect", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      apiServerHandler.conectToWifi(bodyDoc, request);
    }
  );

  server.on(
    "/api/setup/wifi/scanner/start", 
    HTTP_GET, 
    [](AsyncWebServerRequest *request){
      apiServerHandler.scannerWifi(request);
    }
  );

  server.on(
    "/api/setup/wifi/scanner/result", 
    HTTP_GET, 
    [](AsyncWebServerRequest *request){
      apiServerHandler.getScannerWifiResult(request);
    }
  );



  // ========================= POST set time long press =========================
  server.on(
    "/api/setup/timePress/long", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,   
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      String bodyString;
      JsonDocument bodyDoc;
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::setTimeLongPress(bodyDoc, request);
    }
  );

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor HTTP listo.");
}
