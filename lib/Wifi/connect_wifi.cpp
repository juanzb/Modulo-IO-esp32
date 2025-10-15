#include <connect_wifi.hpp>

const char* ssid = "ZARACHE BARRIOS";
const char* password = "FZ11223455";

const char* ssidAP = "ESP32";
const char* passwordAP = "password";

void wifiSetup () {
  WiFi.mode(WIFI_AP_STA);           // Configurar modo AP + STA
  WiFi.softAP(ssidAP, passwordAP);  // Configurar punto de acceso
  Serial.println("Punto de acceso iniciado");
  Serial.print("IP LOCAL AP: ");
  Serial.println(WiFi.softAPIP());

  WiFi.begin(ssid, password); // Conectar a red WiFi
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi conectado");

  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP STA: ");
  Serial.println(WiFi.localIP());
}