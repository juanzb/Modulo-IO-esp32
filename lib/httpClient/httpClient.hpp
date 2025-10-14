#pragma once
#include <Arduino.h>
#include <HTTPClient.h>

class HttpClientESP32 {
public:
    HttpClientESP32(uint16_t timeout = 3000, bool debug = false);
    void setDebug(bool debug);
    void setTimeout(uint16_t timeout);

    // ---------------- GET ----------------
    bool get(const String& url, String& response);

    // ---------------- POST ----------------
    bool post(const String& url, const String& jsonBody, String& response);

private:
    uint16_t _timeout;
    bool _debug;
};
