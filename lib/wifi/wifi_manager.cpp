#include "wifi_manager.h"
#include <WiFi.h>
#include <config.h>

// Credentials come from include/config.h, which is not versioned.
const char* SSID = WIFI_SSID;
const char* PASSWORD = WIFI_PASSWORD;

void WifiManager::begin() {

    WiFi.mode(WIFI_STA);

    WiFi.begin(SSID, PASSWORD);

    Serial.print("Conectando");

    while (WiFi.status() != WL_CONNECTED) {

        Serial.print(".");

        delay(500);
    }

    Serial.println();
    Serial.println("WiFi conectado!");
}

void WifiManager::loop() {

    if (WiFi.status() == WL_CONNECTED)
        return;

    Serial.println("Reconectando WiFi...");

    WiFi.disconnect();

    WiFi.begin(SSID, PASSWORD);
}

bool WifiManager::isConnected() {

    return WiFi.status() == WL_CONNECTED;
}