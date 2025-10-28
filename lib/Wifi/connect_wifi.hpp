#pragma once
#include <WiFi.h>
#include <Arduino.h>
#include <LittleFS.h>

struct ConfigNetwork {
    bool apMode;
    String ssid;
    String pass;
    int deviceID;
};

class WiFiManager {
public:
    WiFiManager();
    void begin();
    void loadConfig();
    bool saveConfig(bool apMode, const String& ssid, const String& pass, int deviceID);
    bool connectToWiFi();
    bool startAccessPoint(const String& apPassword = "12345678"); // Inicia modo AP (opcional contraseña)
    void stopAccessPoint();                                       // Detiene modo AP
    void scanNetworks();
    void reconnectIfNeeded();

private:
    ConfigNetwork config;
};
