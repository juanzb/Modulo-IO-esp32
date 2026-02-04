#pragma once

/*
 * connect_wifi.hpp
 *
 * Interfaz pública de WiFiManager para ESP32
 *
 * Requisitos cubiertos:
 *  - Habilitar/deshabilitar AP manualmente vía API HTTP
 *  - Conectarse automáticamente a la red guardada tras reinicio
 *  - Manejo de reconexión automática
 *  - Escaneo de redes (expuesto por endpoint, seguro contra deadlocks)
 *  - Persistencia de configuración en LittleFS (/config.json)
 *  - API segura para integración con AsyncWebServer (pausar/reanudar si es necesario)
 *
 * Diseño:
 *  - Singleton (WiFiManager::instance()) para que la API HTTP use siempre
 *    la misma instancia compartida y no cree copias temporales.
 *  - Escaneo realizado en tarea FreeRTOS para no bloquear loop()
 *  - Protección simple de concurrencia con portMUX (spinlock)
 *
 * NOTA: Implementación en connect_wifi.cpp (siguiente paso).
 */

#include <Arduino.h>
#include <WiFi.h>
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <vector>
#include <map>

/* Estructura de configuración que se almacena en LittleFS */
struct WiFiConfig {
    bool apMode = true;         // si true, iniciar en AP si no hay SSID válido
    String ssid = "";           // SSID guardado
    String pass = "";           // Pass guardado
    uint32_t deviceID = 0;      // ID del dispositivo (por ejemplo ESP.getEfuseMac())
};

class WiFiManager {
  
  public:
    static WiFiManager& instance();
  
  /* Inicialización */
    void begin();               // Monta LittleFS, carga config, arranca AP o intenta conectar
    void loop();                // Llamar desde loop() -> gestion interna ligera
  
  /* ================== CONTROL AP ================== */
    // Inicia punto de acceso. Devuelve true si fue activado correctamente.
    bool startAccessPoint(const String& apPassword = "");
    
    // Desactiva AP. No borra configuración de apMode, solo apaga la radio AP.
    void stopAccessPoint();
  
  /* ================== CAMBIO DE RED ================== */
    // Cambia la red (guarda en LittleFS y intenta conectar). Retorna true si la conexión fue exitosa.
    bool changeNetwork(const String& ssid, const String& pass);
    
    /* ================== CONEXIÓN / RECONEXIÓN ================== */
    // Forzar reintento de conexión (útil desde API)
    void forceReconnect();
    
    /* ================== PERSISTENCIA ================== */
    // Guarda configuración actual (apMode, ssid, pass, deviceID) en LittleFS
    // Devuelve true si se escribió correctamente.
    bool saveConfig();
    
    // Carga configuración desde LittleFS. Si no existe, deja defaults y crea archivo.
    void loadConfig();
  
    /* ================== UTILIDADES ================== */
    // Obtener nombre del AP (por defecto "ESP32_<deviceID>")
    String getAPName();
    
    // Obtener IP actual
    IPAddress getLocalIP();
    
    // Saber si AP está activo (según estado config/apMode)
    bool isAPActive();
    
    // Obtener estado actual de la configuración en RAM
    WiFiConfig getConfig();
    
  private:

    WiFiManager();
    /* Conexión/gestión interna */
    bool initWiFi();           // intentado seguro de conexión (bloqueante corto)
    bool connectToWiFi();      // lógica para decidir: conectar o levantar AP
    void updateConnection();   // llamado desde loop() para reconexiones
    
    /* Server control interno (si está adjuntado) */
    void stopServer();
    void startServer();
    
    private:
    /* Estado interno */
    WiFiConfig config;
    
    /* Config file path y parámetros */
    const char* CONFIG_PATH = "/config.json";

    /* Tiempo entre reintentos de conexión (ms) y límites */
    const unsigned long CONNECT_TIMEOUT_MS = 8000;
    const unsigned long RECONNECT_DELAY_MS = 5000;

    /* Bandera interna para evitar reconexiones concurrentes */
    volatile bool reconnecting = false;
};
