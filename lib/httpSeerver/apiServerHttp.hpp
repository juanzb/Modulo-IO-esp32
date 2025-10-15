#include <cstdint>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

enum class EnableAPI: uint8_t {
  FALSE = 0,
  TRUE = 1,
};

class ApiServerHttp {
  public:
  ApiServerHttp();

  // -------------- Outputs ------------------
  static void getAmountOutputs(AsyncWebServerRequest *request);
  static void enableStartUpLastState(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setValueStartUpLastState(JsonDocument &docBody, AsyncWebServerRequest *request);


  // -------------- Inputs -------------------
  // Short Press
  static void enableInputSP(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setModeInputSP(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setOutputToInputSP(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setValueModeNormalSP(JsonDocument &docBody, AsyncWebServerRequest *request);
  // Long Press
  static void enableInputLP(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setModeInputLP(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setOutputToInputLP(JsonDocument &docBody, AsyncWebServerRequest *request);
  static void setValueModeNormalLP(JsonDocument &docBody, AsyncWebServerRequest *request);


  // -------------- Time Press -------------------
  static void setTimeLongPress(JsonDocument &docBody, AsyncWebServerRequest *request);
};
