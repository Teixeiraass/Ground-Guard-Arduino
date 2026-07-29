#include "soil_sensor.h"
#include <Arduino.h>

#define SOIL_SENSOR_PIN 34

#define DRY_VALUE 3200
#define WET_VALUE 1300

int SoilSensor::readPercentage()
{
    int raw = analogRead(SOIL_SENSOR_PIN);

    int percentage = map(raw, DRY_VALUE, WET_VALUE, 0, 100);

    percentage = constrain(percentage, 0, 100);

    return percentage;
}