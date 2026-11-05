#include "mqtt_client.h"
#include "command_handler.h"
#include <config.h>
#include <ArduinoJson.h>
#include <Preferences.h>
#include <HTTPClient.h>

#define LED_PIN 2

MqttClient* MqttClient::instance = nullptr;

MqttClient::MqttClient(Client& wifiClient)
    : client(wifiClient) {
    instance = this;
}

void MqttClient::begin() {
    pinMode(LED_PIN, OUTPUT);
    pinMode(34, INPUT);

    Preferences prefs;

    // Device UID (MAC Address sem ':')
    deviceUID = WiFi.macAddress();
    deviceUID.replace(":", "");

    prefs.begin("irrigation", true);

    bool active = prefs.getBool("active", false);

    prefs.end();

    if (active) {
        Serial.println("Restaurando irrigação...");
        digitalWrite(LED_PIN, HIGH);
    } else {
        digitalWrite(LED_PIN, LOW);
    }

    registerDevice();

    client.setServer("192.168.15.3", 1883);
    client.setCallback(callback);
}

void MqttClient::loop() {
    reconnect();

    client.loop();

    if (millis() - lastTelemetry >= 5000)
    {
        lastTelemetry = millis();

        publishTelemetry();
    }
}

void MqttClient::reconnect() {

    if (client.connected())
        return;

    Serial.println("Conectando ao broker...");

    String willTopic = "ground-guard/devices/" + deviceUID + "/state";

    const char* willPayload = "{\"online\":false}";

    // Unique per device: a shared client id makes sensors kick each other off the broker.
    if (client.connect(
        deviceUID.c_str(),
        nullptr,
        nullptr,
        willTopic.c_str(),
        1,
        true,
        willPayload))
    {

        Serial.println("MQTT conectado!");

        Preferences prefs;
        prefs.begin("irrigation", true);

        bool irrigating = prefs.getBool("active", false);

        prefs.end();

        publishState(true, irrigating);

        String topic = "ground-guard/devices/" + deviceUID + "/commands";
        subscribe(topic.c_str());

        Serial.print("Inscrito em: ");
        Serial.println(topic);

    } else {

        Serial.printf("Erro MQTT: %d\n", client.state());
    }
}

bool MqttClient::publish(const char* topic, const char* payload) {
    return client.publish(topic, payload);
}

bool MqttClient::subscribe(const char* topic) {
    return client.subscribe(topic);
}

void MqttClient::callback(char* topic, byte* payload, unsigned int length) {

    String message;

    for (unsigned int i = 0; i < length; i++) {
        message += (char)payload[i];
    }

    auto response = CommandHandler::handle(topic, message);

    if (response.result == CommandResult::SUCCESS) {
        Preferences prefs;
        prefs.begin("irrigation", true);

        bool irrigating = prefs.getBool("active", false);

        prefs.end();

        instance->publishState(true, irrigating);

        instance->publishAck(response.commandId, "SUCCESS");
    } else {
        instance->publishAck(response.commandId, "FAILED");
    }
}

void MqttClient::publishAck(const String& commandId, const char* status)
{
    JsonDocument doc;

    doc["command_id"] = commandId;
    doc["status"] = status;

    String output;
    serializeJson(doc, output);

    String topic = "ground-guard/devices/" + deviceUID + "/events";

    client.publish(topic.c_str(), output.c_str());

    Serial.print("ACK enviado para: ");
    Serial.println(topic);
}

void MqttClient::registerDevice()
{
    HTTPClient http;

    http.begin("http://192.168.15.3:8080/api/v1/devices/register");
    http.addHeader("Content-Type", "application/json");

    String deviceName = "ground_guard_" + deviceUID;

    JsonDocument doc;

    doc["device_uid"] = deviceUID;
    doc["firmware_version"] = "1.0.0";
    doc["firmware_build"] = "1";
    doc["ip_address"] = WiFi.localIP().toString();
    doc["wifi_ssid"] = WiFi.SSID();
    doc["status"] = "ATIVO";
    doc["name"] = deviceName;

    String body;
    serializeJson(doc, body);

    int code = http.POST(body);

    Serial.printf("Register Device: %d\n", code);

    if (code > 0) {
        Serial.println(http.getString());
    } else {
        Serial.printf("Erro HTTP: %d\n", code);
    }

    http.end();
}

void MqttClient::publishState(bool online, bool irrigating)
{
    JsonDocument doc;

    doc["is_online"] = online;
    doc["is_irrigating"] = irrigating;

    String payload;
    serializeJson(doc, payload);

    String topic = "ground-guard/devices/" + deviceUID + "/state";

    client.publish(topic.c_str(), payload.c_str(), true);

    Serial.print("State enviado: ");
    Serial.println(payload);
}

#include "soil_sensor.h"

void MqttClient::publishTelemetry()
{
    JsonDocument doc;

    SoilReading soil = SoilSensor::read();

    // A faulty probe sends no soil_moisture, so the API keeps the last
    // good value instead of recording a fake 0% or 100%.
    if (soil.ok) {
        doc["soil_moisture"] = soil.percentage;
    }
    doc["soil_raw"] = soil.raw;
    doc["sensor_ok"] = soil.ok;
    doc["rssi"] = WiFi.RSSI();

    doc["ip_address"] = WiFi.localIP().toString();
    doc["wifi_ssid"] = WiFi.SSID();

    String payload;
    serializeJson(doc, payload);

    String topic = "ground-guard/devices/" + deviceUID + "/telemetry";

    client.publish(topic.c_str(), payload.c_str());

    Serial.print("Telemetry enviada: ");
    Serial.println(payload);
}