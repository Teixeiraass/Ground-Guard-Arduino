#include "mqtt_client.h"
#include "command_handler.h"
#include <config.h>
#include <ArduinoJson.h>
#include <Preferences.h>

#define LED_PIN 2

MqttClient* MqttClient::instance = nullptr;

MqttClient::MqttClient(Client& wifiClient)
    : client(wifiClient) {
        instance = this;
}

void MqttClient::begin() {
    pinMode(LED_PIN, OUTPUT);


    Preferences prefs;

    prefs.begin("irrigation", true);

    bool active = prefs.getBool("active", false);

    prefs.end();


    if(active)
    {
        Serial.println("Restaurando irrigação...");
        digitalWrite(LED_PIN, HIGH);
    }
    else
    {
        digitalWrite(LED_PIN, LOW);
    }


    client.setServer("192.168.15.15",1883);
    client.setCallback(callback);
}

void MqttClient::loop() {

    reconnect();

    client.loop();
}

void MqttClient::reconnect() {

    if (client.connected())
        return;

    Serial.println("Conectando ao broker...");

    if (client.connect("ground-guard")) {

        Serial.println("MQTT conectado!");

        subscribe("ground-guard/devices/ieystpsd/commands");

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

        instance->publishAck(response.commandId, "SUCCESS");

    } else {

        instance->publishAck(response.commandId, "FAILED");
    }
}

void MqttClient::publishAck(const String& commandId, const char* status)
{
    JsonDocument doc;

    Serial.println("Enviando ACK...");
    Serial.println(commandId);
    Serial.println(status);

    doc["command_id"] = commandId;
    doc["status"] = status;

    String output;
    serializeJson(doc, output);

    client.publish("ground-guard/devices/ieystpsd/events", output.c_str());
}