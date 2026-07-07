#include <WiFi.h>
#include "mqtt_client.h"
#include "wifi_manager.h"

WifiManager wifi;
WiFiClient wifiClient;
MqttClient mqtt(wifiClient);

void setup() {

    Serial.begin(115200);

    wifi.begin();

    mqtt.begin();
}

void loop() {

    wifi.loop();

    mqtt.loop();
}