#pragma once

#include <Arduino.h>

enum class CommandResult {
    SUCCESS,
    FAILED,
    INVALID_COMMAND
};

struct CommandResponse {
    CommandResult result;
    String commandId;
};

class CommandHandler {
public:
    static CommandResponse handle(const String& topic, const String& payload);
};