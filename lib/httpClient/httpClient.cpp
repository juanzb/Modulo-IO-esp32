#include <httpClient.hpp>

class HttpClientESP32 {
public:
    HttpClientESP32(uint16_t timeout = 3000, bool debug = false)
        : _timeout(timeout), _debug(debug) {}

    void setDebug(bool debug) { _debug = debug; }
    void setTimeout(uint16_t timeout) { _timeout = timeout; }

    // ---------------- GET ----------------
    bool get(const String& url, String& response) {
        HTTPClient http;
        unsigned long tStart = millis();

        if (_debug) Serial.printf("[HTTP] GET -> %s\n", url.c_str());
        if (!http.begin(url)) {
            if (_debug) Serial.println("[HTTP] ❌ Fallo al iniciar cliente");
            return false;
        }

        http.addHeader("Connection", "keep-alive");
        http.setTimeout(_timeout);

        int httpCode = http.GET();
        unsigned long tElapsed = millis() - tStart;

        if (_debug) Serial.printf("[HTTP] Código: %d | Duración: %lu ms\n", httpCode, tElapsed);

        if (httpCode > 0 && httpCode == HTTP_CODE_OK) {
            response = http.getString();
            if (_debug) Serial.printf("[HTTP] ✅ Tamaño respuesta: %d bytes\n", response.length());
        } else {
            if (_debug) Serial.printf("[HTTP] ❌ Error en GET (%d)\n", httpCode);
        }

        http.end();
        return (httpCode == HTTP_CODE_OK);
    }

    // ---------------- POST ----------------
    bool post(const String& url, const String& jsonBody, String& response) {
        HTTPClient http;
        unsigned long tStart = millis();

        if (_debug) {
            Serial.printf("[HTTP] POST -> %s\n", url.c_str());
            Serial.printf("[HTTP] Enviando body (%d bytes): %s\n", jsonBody.length(), jsonBody.c_str());
        }

        if (!http.begin(url)) {
            if (_debug) Serial.println("[HTTP] ❌ Fallo al iniciar cliente");
            return false;
        }

        http.addHeader("Content-Type", "application/json");
        http.addHeader("Connection", "keep-alive");
        http.setTimeout(_timeout);

        int httpCode = http.POST(jsonBody);
        unsigned long tElapsed = millis() - tStart;

        if (_debug) Serial.printf("[HTTP] Código: %d | Duración: %lu ms\n", httpCode, tElapsed);

        if (httpCode > 0 && (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED)) {
            response = http.getString();
            if (_debug) Serial.printf("[HTTP] ✅ Tamaño respuesta: %d bytes\n", response.length());
        } else {
            if (_debug) Serial.printf("[HTTP] ❌ Error en POST (%d)\n", httpCode);
        }

        http.end();
        return (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED);
    }

private:
    uint16_t _timeout;
    bool _debug;
};
