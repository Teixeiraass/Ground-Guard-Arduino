#include "command_handler.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <Preferences.h>

bool irrigationActive = false;

CommandResponse CommandHandler::handle(const String& topic, const String& payload)
{
    JsonDocument doc;
    deserializeJson(doc, payload);

    CommandResponse res;
    // The API sends "command_id"; "commandId" kept for older payloads.
    res.commandId = doc["command_id"] | doc["commandId"] | "";

    String action = doc["action"];

    if (action == "START") {

        Serial.println("🌱 Ligando LED (irrigação simulada)");
        digitalWrite(2, HIGH);

        Preferences prefs;
        prefs.begin("irrigation", false);

        prefs.putBool("active", true);

        prefs.end();

        res.result = CommandResult::SUCCESS;
        return res;
    }

    if (action == "STOP") {

        Serial.println("🛑 Desligando LED (irrigação parada)");
        digitalWrite(2, LOW);

        Preferences prefs;
        prefs.begin("irrigation", false);

        prefs.putBool("active", false);

        prefs.end();

        res.result = CommandResult::SUCCESS;
        return res;
    }

    res.result = CommandResult::INVALID_COMMAND;
    return res;
}