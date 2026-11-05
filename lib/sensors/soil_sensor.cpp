#include "soil_sensor.h"
#include <Arduino.h>

#define SOIL_SENSOR_PIN 34

#define DRY_VALUE 3200
#define WET_VALUE 1300

// The calibrated range is 1300..3200. Readings pinned near the ADC rails
// mean the probe is unplugged (pin floats/pulls to 4095) or shorted (~0),
// not real soil, so they are flagged instead of reported as 0% or 100%.
#define FAULT_LOW_RAW 200
#define FAULT_HIGH_RAW 3900

#define SAMPLE_COUNT 16
#define SAMPLE_DELAY_MS 2

SoilReading SoilSensor::read()
{
    long sum = 0;
    int minRaw = 4095;
    int maxRaw = 0;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        int sample = analogRead(SOIL_SENSOR_PIN);
        sum += sample;
        if (sample < minRaw) minRaw = sample;
        if (sample > maxRaw) maxRaw = sample;
        delay(SAMPLE_DELAY_MS);
    }

    // Drop the extremes to filter ADC spikes, then average the rest.
    int raw = (int)((sum - minRaw - maxRaw) / (SAMPLE_COUNT - 2));

    SoilReading reading;
    reading.raw = raw;
    reading.ok = raw > FAULT_LOW_RAW && raw < FAULT_HIGH_RAW;

    int percentage = map(raw, DRY_VALUE, WET_VALUE, 0, 100);
    reading.percentage = constrain(percentage, 0, 100);

    return reading;
}

int SoilSensor::readPercentage()
{
    return read().percentage;
}
