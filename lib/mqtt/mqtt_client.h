#ifndef MQTT_CLIENT_H
#define MQTT_CLIENT_H

#include <PubSubClient.h>
#include <WiFi.h>

static constexpr int LED_PIN = 2;

class MqttClient {
public:
    MqttClient(Client& wifiClient);

    void begin();
    void loop();

    bool publish(const char* topic, const char* payload);
    bool subscribe(const char* topic);

private:
    PubSubClient client;
    String deviceUID;
    unsigned long lastTelemetry = 0;

    static MqttClient* instance;

    void reconnect();

    static void callback(char* topic, byte* payload, unsigned int length);
    void publishAck(const String& commandId, const char* status);
    void registerDevice();
    void publishState(bool online, bool irrigating);
    void publishTelemetry();
};

#endif