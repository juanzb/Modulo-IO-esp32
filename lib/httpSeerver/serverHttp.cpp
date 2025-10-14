#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <handleOutput.hpp>
#include <struct_io.hpp>
#include <apiServerHttp.hpp>


AsyncWebServer server(80);

// ─────────────── Función para parsear JSON ───────────────
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


// ─────────────────────── setup ───────────────────────
void setupHttpServer() {
  // ───────────── Ruta para recibir JSON ─────────────
  server.on(
    "/api/action/relay", 
    HTTP_POST, 
    [](AsyncWebServerRequest *request){}, 
    NULL,   
    [](AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total) {
      Serial.println("SOLICITUD HTTP RECIBIDA");
      String bodyString;
      JsonDocument bodyDoc = DynamicJsonDocument(512);
      parseBody(request, data, len, bodyString, bodyDoc);
      ApiServerHttp::setValueStartUpLastState(bodyDoc, request);
    }
  );

  // ───────────── Ruta simple GET ─────────────
  server.on("/api/test", HTTP_GET, [](AsyncWebServerRequest *req){
    req->send(200, "application/json", "{\"pong\":true}");
  });

  // Iniciar servidor
  server.begin();
  Serial.println("Servidor HTTP listo.");
}
