#include "connect_wifi.hpp"
#include <ArduinoJson.h>

WiFiManager::WiFiManager() {
	config.deviceID = (uint32_t)ESP.getEfuseMac();
}

void WiFiManager::begin() {
	if (!LittleFS.begin(true)) {
		Serial.println("[WiFiManager] Error montando LittleFS");
		return;
	}

	loadConfig();

	if (config.apMode || config.ssid.isEmpty()) {
		startAccessPoint();
	} else {
		connectToWiFi();
	}
}

bool WiFiManager::initWiFi() {
	if (WiFi.status() == WL_CONNECTED) {
		Serial.println("[WiFiManager] Ya conectado a WiFi");
		return true;
	}

	WiFi.mode(WIFI_STA);
	WiFi.begin(config.ssid.c_str(), config.pass.c_str());

	unsigned long start = millis();
	const unsigned long timeout = 8000; // 8 segundos

	Serial.printf("[WiFiManager] Conectando a SSID: %s ...\n", config.ssid.c_str());

	while (WiFi.status() != WL_CONNECTED && millis() - start < timeout) {
		delay(250);
		Serial.print(".");
	}

	startAccessPoint();

	if (WiFi.status() == WL_CONNECTED) {
		Serial.printf("\n[WiFiManager] Conectado! IP: %s\n", WiFi.localIP().toString().c_str());
		return true;
	} else {
		Serial.println("\n[WiFiManager] No se pudo conectar, activando AP...");
		return false;
	}
}

std::vector<std::map<String, String>> WiFiManager::scanNetworks() {
	std::vector<std::map<String, String>> networksFound = {};
	Serial.println("Scan start");
  int AmountNetworsFound = WiFi.scanNetworks(); 	// WiFi.scanNetworks will return the number of networks found.
	
  if (AmountNetworsFound == 0) {
		Serial.println("no networks found");
		return networksFound;
  }
	
	Serial.println(AmountNetworsFound + " networks found");
	Serial.println("Nr | SSID | RSSI | CH | Encryption");

	for (int i = 0; i < AmountNetworsFound; ++i) {
		// Print SSID and RSSI for each network found
		Serial.printf("%2d", i + 1);
		Serial.print(" | ");
		Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
		Serial.print(" | ");
		Serial.printf("%4ld", WiFi.RSSI(i));
		Serial.print(" | ");
		Serial.printf("%2ld", WiFi.channel(i));
		Serial.print(" | ");
		switch (WiFi.encryptionType(i)) {
			case WIFI_AUTH_OPEN:            Serial.print("open"); break;
			case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
			case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
			case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
			case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
			case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
			case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
			case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
			case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
			default:                        Serial.print("unknown");
		}

		networksFound.push_back({
			{"ssid", WiFi.SSID(i).c_str()},
			{"rssi", String(WiFi.RSSI(i))},
			{"channel", String(WiFi.channel(i))},
			{"encryption", String(WiFi.encryptionType(i))}
		});

		Serial.println();
		delay(10);
  }

  WiFi.scanDelete(); 	// Delete the scan result to free memory for code below.
	return networksFound;
}


bool WiFiManager::startAccessPoint(const String& apPassword) {
	String apName = getAPName();
	WiFi.mode(WIFI_AP_STA);
	
	if (WiFi.getMode() == WIFI_AP) {
		Serial.println("[WiFiManager] Modo AP-STA activado");
		return true;
	}
	else {
		bool enableAP = WiFi.softAP(apName.c_str(), apPassword.c_str());
		if (enableAP) {
			Serial.printf("[WiFiManager] AP activado: %s | IP: %s\n", apName.c_str(), WiFi.softAPIP().toString().c_str());
			config.apMode = true;
		} else {
			Serial.println("[WiFiManager] Error al activar AP");
		}
		return enableAP;
	}

}

void WiFiManager::stopAccessPoint() {
	WiFi.softAPdisconnect(true);
	Serial.println("[WiFiManager] AP desactivado");
	config.apMode = false;
}

bool WiFiManager::connectToWiFi() {
	if (config.ssid.isEmpty()) {
		Serial.println("[WiFiManager] SSID vacío, activando AP...");
		startAccessPoint();
		return false;
	}

	std::vector<std::map<String, String>> networksFound = scanNetworks();

	// Verifica si la red está disponible
	bool found = false;
	for (std::map<String, String> network : networksFound) {
		if (network["ssid"] == config.ssid) {
			found = true;
			break;
		}
	}

	for (int i = 0; i < WiFi.scanComplete(); ++i) {
		if (WiFi.SSID(i) == config.ssid) {
			found = true;
			break;
		}
	}

	if (!found) {
		Serial.println("[WiFiManager] SSID no encontrado, activando AP...");
		startAccessPoint();
		return false;
	}

	return initWiFi();
}

void WiFiManager::reconnectIfNeeded() {
	if (WiFi.status() != WL_CONNECTED && !config.apMode) {
		Serial.println("[WiFiManager] WiFi desconectado, intentando reconectar...");
		connectToWiFi();
	}
}

bool WiFiManager::changeNetwork(const String& ssid, const String& pass) {
	config.ssid = ssid;
	config.pass = pass;

	if (!saveConfig()) {
		Serial.println("[WiFiManager] Error guardando nueva configuración");
		return false;
	}

	return connectToWiFi();
}

// --- PRIVATE ---

void WiFiManager::loadConfig() {
	if (!LittleFS.exists("/config.json")) {
		Serial.println("[WiFiManager] No existe config.json, activando AP por defecto");
		config.apMode = true;
		saveConfig();
		return;
	}

	File f = LittleFS.open("/config.json", "r");
	if (!f) {
		Serial.println("[WiFiManager] Error abriendo config.json");
		config.apMode = true;
		return;
	}

	DynamicJsonDocument doc(512);
	DeserializationError err = deserializeJson(doc, f);
	f.close();

	if (err) {
		Serial.println("[WiFiManager] Error leyendo config.json, activando AP");
		config.apMode = true;
		return;
	}

	config.apMode = doc["apMode"] | true;
	config.ssid = doc["ssid"] | "";
	config.pass = doc["pass"] | "";
	config.deviceID = doc["deviceID"] | (uint32_t)ESP.getEfuseMac();
}

bool WiFiManager::saveConfig() {
	File f = LittleFS.open("/config.json", "w");
	if (!f) {
		Serial.println("[WiFiManager] No se pudo abrir config.json para escritura");
		return false;
	}

	DynamicJsonDocument doc(512);
	doc["apMode"] = config.apMode;
	doc["ssid"] = config.ssid;
	doc["pass"] = config.pass;
	doc["deviceID"] = config.deviceID;

	if (serializeJson(doc, f) == 0) {
		Serial.println("[WiFiManager] Error serializando JSON");
		f.close();
		return false;
	}

	f.close();
	Serial.println("[WiFiManager] Configuración guardada");
	return true;
}

String WiFiManager::getAPName() {
	return "ESP32_" + String(config.deviceID, HEX);
}

