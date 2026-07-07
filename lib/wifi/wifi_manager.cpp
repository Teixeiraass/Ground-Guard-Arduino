#include "wifi_manager.h"
#include <WiFi.h>

const char* SSID = "janny";
const char* PASSWORD = "re70488705";

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