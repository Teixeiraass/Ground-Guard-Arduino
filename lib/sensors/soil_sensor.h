#pragma once

struct SoilReading {
    int raw;         // averaged 12-bit ADC value
    int percentage;  // 0 (dry) .. 100 (wet), meaningful only when ok
    bool ok;         // false when the reading looks like a disconnected/shorted probe
};

class SoilSensor {
public:
    static SoilReading read();
    static int readPercentage();
};
