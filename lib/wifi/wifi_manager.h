#pragma once

class WifiManager {
public:
    void begin();
    void loop();

    bool isConnected();
};