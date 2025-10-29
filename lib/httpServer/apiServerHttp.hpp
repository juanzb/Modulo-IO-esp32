#include <cstdint>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

class ApiServerHttp {
  public:
  ApiServerHttp();

  // -------------- Outputs ------------------
  static void getAmountOutputs(AsyncWebServerRequest *request);
  static void writeValueOutput(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setValueStartUp(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void enableStartUpLastState(JsonDocument &docBody, AsyncWebServerRequest *request);

  // -------------- Inputs -------------------
  static void getAmountInputs(AsyncWebServerRequest *request);
  static void enableInput(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setModeInput(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setOutputToInput(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setValueModeNormalInput(JsonDocument &docBody, AsyncWebServerRequest *request);

  // -------------- Wifi -------------------
  static void accesPointWifi(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void conectToWifi(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void scannerWifi(JsonDocument &docBody, AsyncWebServerRequest *request);

  // -------------- Time Press -------------------
  static void setTimeLongPress(JsonDocument &docBody, AsyncWebServerRequest *request);
};
