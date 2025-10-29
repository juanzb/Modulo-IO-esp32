#pragma once

#include <WiFi.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

struct WiFiConfig {
    bool apMode = true;
    String ssid = "";
    String pass = "";
    uint32_t deviceID = 0;
};

class WiFiManager {
public:
    WiFiManager();

    void begin();                     // Inicializa WiFi según configuración
    bool initWiFi();                   // Inicializa WiFi STA
    String scanNetworks();               // Escanea redes disponibles
    bool startAccessPoint(const String& apPassword = "12345678"); // Activa AP
    void stopAccessPoint();            // Desactiva AP
    bool connectToWiFi();              // Conecta a la red configurada
    void reconnectIfNeeded();          // Reconexión controlada
    bool changeNetwork(const String& ssid, const String& pass); // Cambia red WiFi

private:
    WiFiConfig config;

    void loadConfig();                 // Carga configuración de LittleFS
    bool saveConfig();                 // Guarda configuración en LittleFS
    String getAPName();                // Genera nombre único AP
};
